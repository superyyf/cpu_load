#include "load_thread.h"


LoadThread::LoadThread(int index){
    index_ = index;

}

void LoadThread::load_fn(){
    int i = 0;
    while(!exit_){
        if(!mutex_.try_lock()){
            continue;
        }
    }
}