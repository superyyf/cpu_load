#include "cpu_load.h"
#include <unistd.h>
#include <iostream>
#include <mntent.h>
#include <sys/io.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstring>
#include "load_compute.h"
#include <fstream>

#define MOUNT_INFO_FILE "/proc/mounts"
#define CGROUP_NAME "cpu_load"
#define CPU_DEFAULT_MAX 1000000

CpuLoad::CpuLoad()
    : inited_(false)
    , exit_flag_(false)
    , cpu_num_(0)
    , cgroup_version_(CGROUP_NONE){
    
}

CpuLoad::~CpuLoad(){
    if(keep_thread_.joinable()){
        keep_thread_.join();
    }
}

void CpuLoad::set_cpu_load(load_value load){
    std::lock_guard<std::mutex> lock(expect_mutex_);
    expect_load_ = load;
}

bool CpuLoad::Init(){
    /*check cgroup v1 or v2 is ready*/
    // TODO
    if(inited_){
        return false;
    }
    cpu_num_ = sysconf(_SC_NPROCESSORS_CONF);
    cgroup_version_ = get_cgroup_version();
    if(cgroup_version_ == CGROUP_NONE){
        std::cout << "error: need cgroup" << std::endl;
        return false;
    }
    cgroup_mount_point_ = get_cgroup_path();
    if(cgroup_mount_point_.empty()){
        return false;
    }

    //add cpu cpuset to cgroup.subtree_control
    std::string sub_ctrller = cgroup_mount_point_ + "/cgroup.subtree_control";
    std::ofstream file;
    file.open(sub_ctrller);
    if(!file.is_open()){
        std::cout << "open: " << strerror(errno) << std::endl;
        return false; 
    }
    file << "+cpu " << "+cpuset";
    file.close();
    
    group_path_ = cgroup_mount_point_ + "/" + CGROUP_NAME;
    if(access(group_path_.c_str(), 0) == -1){
        int ret = mkdir(group_path_.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
        if(ret != 0){
            std::cout << "mkdir: " << strerror(errno) << std::endl;
            return false;
        }
    }

    set_load_limit(0);

    for(int i = 0; i < cpu_num_; ++i){
        std::shared_ptr<LoadThread> th = std::make_shared<LoadThread>(i, group_path_+"/thread_"+std::to_string(i));
        th->Init();
        load_threads_.insert({i, th});
    }

    keep_thread_ = std::thread(&CpuLoad::keep_load_fn, this);

    inited_ = true;
    return true;
}

void CpuLoad::Run(){
    for(auto &t : load_threads_){
        t.second->Run();
    }
}

void CpuLoad::Stop(){
    exit_flag_ = true;
    for(auto &t : load_threads_){
        t.second->Stop();
    }
    if(rmdir(group_path_.c_str()) == -1){
        std::cout << "rmdir: " << strerror(errno) << std::endl;
    }
}

CgroupVersion CpuLoad::get_cgroup_version(){
    // TODO
    return CGROUP_V2;
}

void CpuLoad::print_load_info(load_value now, load_value expect){
    std::cout << "load now: " << now << "%, expect load: " << expect <<"%" << std::endl;
}

void CpuLoad::keep_load_fn(){

    while(!exit_flag_){
        load_rate now_load;
        do{
            std::lock_guard<std::mutex> lock(real_mutex_);
            real_load_ = get_sysCpuUsage();
            now_load = real_load_;
        }while(0);

        print_load_info(now_load, expect_load_);

        int dlt = expect_load_ - now_load;
    
        if(dlt >= 5){
            dlt = 5;
        }
        else if(dlt < -5){
            dlt = -5;
        }

        limit_ = limit_ + dlt;
        limit_ = (limit_ > expect_load_) ? expect_load_ : limit_;
        limit_ = (limit_ < 0) ? 0 : limit_; 
        set_load_limit(limit_);
    }
}

bool CpuLoad::set_load_limit(load_value limit){
    if(limit < 0 || limit > 100){
        std::cout << "limit not suitable" << std::endl;
        return false;
    }
    std::string path = group_path_ + "/cpu.max";
    std::ofstream file;
    int time_limit = (int)(((double)limit / (double)100) * (double)CPU_DEFAULT_MAX);
    if(time_limit ==  0){
        time_limit = (int)((double)0.001 * (double)CPU_DEFAULT_MAX);
    }

    time_limit *= cpu_num_;
    std::string value = std::to_string(time_limit) + " " + std::to_string(CPU_DEFAULT_MAX);
    file.open(path);
    if(!file.is_open()){
        std::cout << "open: " << strerror(errno) << std::endl;
        return false;
    }

    file << value;
    file.close();
    return true;
} 

std::string CpuLoad::get_cgroup_path(){
    struct mntent *mnt;
    FILE *info = setmntent(MOUNT_INFO_FILE, "r");
    if(info == nullptr){
        std::cerr << "setmntent error" << std::endl;
        return "";
    }

    while((mnt = getmntent(info)) != nullptr){
        if(strcmp(mnt->mnt_type,"cgroup2") == 0){
            std::string res(mnt->mnt_dir);
            endmntent(info);
            return res;
        }
    }
    endmntent(info);
    return "";
}