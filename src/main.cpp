#include "cpu_load.h"
#include <unistd.h>
#include <csignal>
#include <iostream>

bool exit_flag = false;

void signal_handler(int sig){
    std::cout << "receive a signal" << std::endl;
    exit_flag = true;
}

int main(int argc, char* argv[]){
    
    signal(SIGINT, signal_handler);
    CpuLoad* loader = CpuLoad::get_instance();
    loader->Init();
    loader->set_cpu_load(80);
    loader->Run();

    while(!exit_flag){
        sleep(1);
    }

    loader->Stop();
}