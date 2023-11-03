#include "cpu_load.h"
#include <unistd.h>
#include <iostream>
#include <mntent.h>
#include <sys/io.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstring>

#define MOUNT_INFO_FILE "/proc/mounts"
#define CGROUP_NAME "cpu_load"

CpuLoad::CpuLoad()
    : inited_(false)
    , exit_flag_(true)
    , cpu_num_(0)
    , cgroup_version_(CGROUP_NONE){
    
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
    std::string cpath = get_cgroup_path();
    if(cpath.empty()){
        return false;
    }
    
    cgroup_path_ = cpath + "/" + CGROUP_NAME;
    if(access(cgroup_path_.c_str(), 0) == -1){
        int ret = mkdir(cgroup_path_.c_str(), S_IRGRP | S_IWGRP);
        if(ret != 0){
            std::cout << "mkdir: " << strerror(errno) << std::endl;
            return false;
        }
    }

    for(int i = 0; i < cpu_num_; ++i){
        std::shared_ptr<LoadThread> th = std::make_shared<LoadThread>(i, cgroup_path_+"/thread_"+std::to_string(i));
        th->Init();
        load_threads_.insert({i, th});
    }

    inited_ = true;
    return true;
}

void CpuLoad::Run(){
    for(auto &t : load_threads_){
        t.second->Run();
    }
}

void CpuLoad::Stop(){
    for(auto &t : load_threads_){
        t.second->Stop();
    }
}


CgroupVersion CpuLoad::get_cgroup_version(){
    // TODO
    return CGROUP_V2;
}

void CpuLoad::load_fn(){
    int i = 0;
    while(!exit_flag_){
        i++;
    }
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