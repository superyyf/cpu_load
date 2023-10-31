#include "cpu_load.h"
#include <unistd.h>
#include <iostream>

CpuLoad::CpuLoad()
    : inited_(false)
    , exit_flag_(true)
    , cpu_num_(0)
    , cgroup_version_(CGROUP_NONE){
    
}

bool CpuLoad::set_cpu_load(load_value load){
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

    
    inited_ = true;
    return true;
}

void CpuLoad::Run(){
    for(int i = 0; i < cpu_num_; ++i){
        if(load_threads_.find(i) == load_threads_.end()){
            load_threads_.insert({i, std::thread(load_fn)});
        }
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