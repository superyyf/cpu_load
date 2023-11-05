#ifndef CPU_LOAD_H_
#define CPU_LOAD_H_
#include <mutex>
#include <thread>
#include <map>
#include <atomic>
#include <load_thread.h>


typedef int load_value;
typedef unsigned char load_rate;

enum CgroupVersion {
    CGROUP_NONE = 0,
    CGROUP_V1,
    CGROUP_V2 
};

class CpuLoad{
private:
    bool inited_;
    std::atomic_bool exit_flag_;
    int cpu_num_;
    std::string cgroup_mount_point_;
    std::string group_path_;

    CgroupVersion cgroup_version_;
    load_value expect_load_;
    std::mutex expect_mutex_;
    load_value real_load_;
    std::mutex real_mutex_;
    std::thread keep_thread_;
    load_value limit_;
    std::map<int, std::shared_ptr<LoadThread>> load_threads_;

    CpuLoad();
    CgroupVersion get_cgroup_version();
    std::string get_cgroup_path();
    void keep_load_fn();
    bool set_load_limit(load_value limit);
    void print_load_info(load_value now, load_value expect);

public:
    static CpuLoad* get_instance(){
        static CpuLoad cpu_load_instance;
        return &cpu_load_instance;
    }
    ~CpuLoad();

    bool Init();
    void Run();
    void Stop();

    load_value get_cpu_load();
    void set_cpu_load(load_value load);
};


#endif //CPU_LOAD_H_
