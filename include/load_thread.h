#ifndef LOAD_THREAD_
#define LOAD_THREAD_
#include <set>
#include <thread>
#include <mutex>
#include <atomic>

class LoadThread{
private:
    int index_;
    std::atomic_bool exit_;
    std::set<int> cpu_set_;
    std::thread thread_;
    std::mutex mutex_;
    

public:
    LoadThread(int index);
    LoadThread(int index, std::initializer_list<int> cpu_set);

    void Stop();
    void Start();
    void Pause();
    void Resume();
    void load_fn();
};

#endif // !LOAD_THREAD_
