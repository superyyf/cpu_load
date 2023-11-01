#ifndef LOAD_THREAD_
#define LOAD_THREAD_
#include <set>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

class LoadThread{
private:
    int index_;
    bool run_flag_;
    std::string group_name_;
    std::atomic_bool exit_flag_;
    std::set<int> cpu_set_;
    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable cond_;
    

public:
    LoadThread() = delete;
    LoadThread(int index, std::string group_name_);
    LoadThread(int index, std::string group_name_, std::initializer_list<int> cpu_set);

    bool Init();
    void Stop();
    void Run();
    void Pause();
    void Resume();
    void load_fn();
};

#endif // !LOAD_THREAD_
