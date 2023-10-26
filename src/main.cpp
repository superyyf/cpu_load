#include "cpu_load.h"
#include <unistd.h>
#include <csignal>
#include <iostream>

bool exit_flag;

void signal_handler(){
    std::cout << "receive a signal" << std::endl;
}

int main(int argc, char* argv[]){
    CpuLoad* loader = CpuLoad::get_instance();
    loader->Init();
    loader->set_cpu_load(80);
    while(!exit_flag){
        sleep(1);
    }


}