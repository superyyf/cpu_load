#ifndef	LOAD_COMPUTE_ 
#define LOAD_COMPUTE_ 

typedef struct {
	unsigned long user;
	unsigned long nice;
	unsigned long system;
	unsigned long idle;
}Total_Cpu_Occupy_t;
 
 
typedef struct {
	unsigned int pid;
	unsigned long utime;  //user time
	unsigned long stime;  //kernel time
	unsigned long cutime; //all user time
        unsigned long cstime; //all dead time
}Proc_Cpu_Occupy_t;

typedef struct cpu_occupy_
{
	char name[20];
	unsigned int user;
	unsigned int nice;
	unsigned int system;
	unsigned int idle;
	unsigned int iowait;
	unsigned int irq;
	unsigned int softirq;
} cpu_occupy_t;

double cal_cpuoccupy(cpu_occupy_t *o, cpu_occupy_t *n);
unsigned long get_cpu_total_occupy();
unsigned long get_cpu_proc_occupy(unsigned int pid);
float get_proc_cpu(unsigned int pid);
void get_cpuoccupy(cpu_occupy_t *cpust);
unsigned char get_sysCpuUsage();

#endif // !LOAD_COMPUTE_