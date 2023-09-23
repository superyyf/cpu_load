#ifndef CPU_LOAD_H_
#define CPU_LOAD_H_

typedef int load_value;

class CpuLoad{
private:
    load_value expect_load_;
    load_value real_load_;
    bool inited_;

    CpuLoad();

public:
    static CpuLoad* get_instance(){
        static CpuLoad cpu_load_instance;
        return &cpu_load_instance;
    }

    ~CpuLoad();
    bool Init();
    bool Run();
    bool Stop();

    load_value get_cpu_load();
    bool set_cpu_load(load_value load);
};


#endif //CPU_LOAD_H_
