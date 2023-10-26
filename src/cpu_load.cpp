#include "cpu_load.h"

CpuLoad::CpuLoad()
    : inited_(false)
    , cgroup_version_(CGROUP_V1){
    
}

bool CpuLoad::set_cpu_load(load_value load){
    std::lock_guard<std::mutex> lock(expect_mutex_);
    expect_load_ = load;
}

void CpuLoad::Init(){
    /*check cgroup v1 or v2 is ready*/
    // TODO

    

}