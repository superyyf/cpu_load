#ifndef CPU_LOAD_H_
#define CPU_LOAD_H_
#include <mutex>
#include <thread>
#include <vector>


typedef int load_value;
enum CgroupVersion {
    CGROUP_NONE = 0,
    CGROUP_V1,
    CGROUP_V2 
};

class CpuLoad{
private:
    bool inited_;
    CgroupVersion cgroup_version_;
    load_value expect_load_;
    std::mutex expect_mutex_;
    load_value real_load_;
    std::mutex real_mutex_;
    std::thread load_compute_thread_;
    std::vector<std::thread> load_sumulate_threads_;

    CpuLoad();

public:
    static CpuLoad* get_instance(){
        static CpuLoad cpu_load_instance;
        return &cpu_load_instance;
    }

    ~CpuLoad();
    void Init();
    bool Run();
    bool Stop();

    load_value get_cpu_load();
    bool set_cpu_load(load_value load);
};


#endif //CPU_LOAD_H_
