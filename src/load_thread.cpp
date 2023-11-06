#include "load_thread.h"
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <fstream>
#include <cstring>


LoadThread::LoadThread(int index, std::string group_name)
    : index_(index)
    , run_flag_(false)
    , exit_flag_(false)
    , group_path_(group_name){}

bool LoadThread::Init(){
    
    if(access(group_path_.c_str(), 0) == -1){

        int ret = mkdir(group_path_.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
        if(ret != 0){
            std::cout << "mkdir: " << strerror(errno) << std::endl;
            return false;
        }
    }
    thread_ = std::thread(&LoadThread::load_fn, this);
    return true;
}

void LoadThread::Run(){
    Resume();
}

void LoadThread::Stop(){
    exit_flag_ = true;
    run_flag_ = true;
    cond_.notify_all();
    if(thread_.joinable()){
        thread_.join();
    }
    if(rmdir(group_path_.c_str()) == -1){
        std::cout << "rmdir: " << strerror(errno) << std::endl;
    }
}

void LoadThread::Resume(){
    if(!run_flag_){
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
        std::cout << "open: " << strerror(errno) << std::endl;
    }
}

void LoadThread::load_fn(){
    int i = 0;
    write_proc();

    while(!exit_flag_){
        if(!run_flag_){
            std::unique_lock<std::mutex> lock(mutex_, std::defer_lock);
            cond_.wait(lock, [this]()->bool{ return this->run_flag_; });
        }
        i++;
    }
}