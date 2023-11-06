#include "cpu_load.h"
#include <unistd.h>
#include <csignal>
#include <iostream>
#include <cstring>
#include <atomic>

std::atomic_bool exit_flag = false;

void block_all_signal(){
    sigset_t sig_set;
    if(sigfillset(&sig_set) == -1){
        std::cout <<"sigfillset: " << strerror(errno) << std::endl;
    }
    if(sigprocmask(SIG_SETMASK, &sig_set, nullptr) == -1){
        std::cout <<"sigprocmask: " << strerror(errno) << std::endl;
    }

}

void wait_signal() {
    sigset_t signal_set;
    int sig = -1;
    if(sigemptyset(&signal_set) == -1){ /* empty the set of signals */ 
        std::cout <<"sigemptyset: " << strerror(errno) << std::endl;
    }        
    if(sigaddset(&signal_set, SIGTERM) == -1){ /* Add only SIGTERM to set */
        std::cout <<"sigaddset: " << strerror(errno) << std::endl;
    } 
    if(sigaddset(&signal_set, SIGINT) == -1){
        std::cout <<"sigaddset: " << strerror(errno) << std::endl;
    }
    
    while ((sig != SIGTERM) && (sig != SIGINT)) {
        sigwait(&signal_set, &sig);
    }
    exit_flag = true;
    std::cout << "Goodbye!" << std::endl;
}

int main(int argc, char **argv){

    int c;
    load_rate load = -1;
    while((c = getopt(argc, argv, "d:")) != -1){
        switch(c){
            case 'd':{
                load = atoi(optarg);
                break;
            }
            case '?':
            default:{
                std::cout << "Usage: " << argv[0] << " -d <load_value,rang[0~100]>" << std::endl;
                exit(EXIT_FAILURE);
            }
            
        }
    }
    if(optind < argc){
        std::cout << "Usage: " << argv[0] << " -d <load_value,rang[0~100]>" << std::endl;
        exit(EXIT_FAILURE);
    }

    if(geteuid() != 0){
        std::cout <<"Require root privilege. try 'sudo + cmd'" << std::endl;
        exit(EXIT_FAILURE);
    }

    if(load < 0 || load > 100){
        std::cout << load << ": Load value out of rang! rang[0~100]" << std::endl;
        exit(EXIT_FAILURE);
    }

    block_all_signal();
    std::thread wait_thread(wait_signal);

    CpuLoad* loader = CpuLoad::get_instance();
    if(!loader->Init()){
        std::cout << "init error" << std::endl;
    }
    loader->set_cpu_load(load);
    loader->Run();

    while(!exit_flag){
        sleep(1);
    }

    loader->Stop();

    if(wait_thread.joinable()){
        wait_thread.join();
    }
}