#include "load_thread.h"
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <fstream>


LoadThread::LoadThread(int index, std::string group_name)
    : index_(index)
    , run_flag_(false)
    , exit_flag_(false)
    , group_path_(group_name){}

bool LoadThread::Init(){
    thread_ = std::thread(&LoadThread::load_fn, this);
    int ret = mkdir(group_path_.c_str(), S_IRGRP | S_IWGRP);
    if(ret != 0){
        std::cerr << "mkdir error" << std::endl;
        return false;
    }
    return true;
}

void LoadThread::Run(){
    Resume();
}

void LoadThread::Stop(){
    exit_flag_ = true;
    run_flag_ = true;
    cond_.notify_all();
}

void LoadThread::Resume(){
    if(!run_flag_){
        std::unique_lock<std::mutex> lock(mutex_);
        run_flag_ = true;
        cond_.notify_one();
    }
    else{
        std::cout << "load thread is already running" << std::endl;
    }
}

void LoadThread::Pause(){
    if(run_flag_){
        run_flag_ = false;
    }
    else{
        std::cout << "load thread is not running" << std::endl;
    }
}

void LoadThread::write_proc(){
    pid_t tid = syscall(SYS_gettid);
    std::string proc_path = group_path_ + "/cgroup.procs";
    std::ofstream file;
    file.open(proc_path);
    if(file.is_open()){
        file << tid;
        file.close();
    }
    else{
        std::cerr << "open file error" << std::endl;
    }
}

void LoadThread::load_fn(){
    int i = 0;
    write_proc();

    std::unique_lock<std::mutex> lock(mutex_);
    while(!exit_flag_){
        cond_.wait(lock, [this]()->bool{ return this->run_flag_; });
        i++;
    }
}