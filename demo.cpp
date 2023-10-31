#include <thread>
#include <vector>
void thread_fn(){
 int i = 0;

    while(1){
       i++; 
    }

}

int main(){
    std::vector<std::thread> thread_pool;
    for(int i = 0; i < 4; ++i){
        thread_pool.push_back(std::thread(thread_fn));
    }

    for(auto &f : thread_pool){
        if(f.joinable()){
            f.join();
        }
    }
}