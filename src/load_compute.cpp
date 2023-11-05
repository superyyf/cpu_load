#include "load_compute.h"
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <stdlib.h>

#define PROCESS_ITEM 14

//获取第N项开始的指针
const char* get_items(const char*buffer ,unsigned int item){
	
	const char *p =buffer;
 
	int len = strlen(buffer);
	int count = 0;
	
	for (int i=0; i<len;i++){
		if (' ' == *p){
			count ++;
			if(count == item -1){
				p++;
				break;
			}
		}
		p++;
	}
 
	return p;
}

double cal_cpuoccupy(cpu_occupy_t *o, cpu_occupy_t *n)
{
	double od, nd;
	double id, sd;
	double cpu_use;

	od = (double)(o->user + o->nice + o->system + o->idle + o->softirq + o->iowait + o->irq); //第一次(用户+优先级+系统+空闲)的时间再赋给od
	nd = (double)(n->user + n->nice + n->system + n->idle + n->softirq + n->iowait + n->irq); //第二次(用户+优先级+系统+空闲)的时间再赋给od
	id = (double)(n->idle);																	  //用户第一次和第二次的时间之差再赋给id
	sd = (double)(o->idle);																	  //系统第一次和第二次的时间之差再赋给sd
	if ((nd - od) != 0)
	{
		cpu_use = 100.0 - ((id - sd)) / (nd - od) * 100.00; //((用户+系统)乖100)除(第一次和第二次的时间差)再赋给g_cpu_used
	}
	else
	{
		cpu_use = 0;
	}

	return cpu_use;
}

//获取总的CPU时间
unsigned long get_cpu_total_occupy(){
	
	FILE *fd;
	char buff[1024]={0};
	Total_Cpu_Occupy_t t;
 
	fd =fopen("/proc/stat","r");
	if (nullptr == fd){
		return 0;
	}
		
	fgets(buff,sizeof(buff),fd);
	char name[64]={0};
	sscanf(buff,"%s %ld %ld %ld %ld",name,&t.user,&t.nice,&t.system,&t.idle);
	fclose(fd);
	
	return (t.user + t.nice + t.system + t.idle);
}

//获取进程的CPU时间
unsigned long get_cpu_proc_occupy(unsigned int pid){
	
	char file_name[64]={0};
	Proc_Cpu_Occupy_t t;
	FILE *fd;
	char line_buff[1024]={0};
	sprintf(file_name,"/proc/%d/stat",pid);
	
	fd = fopen(file_name,"r");
	if(nullptr == fd){
		return 0;
	}
	
	fgets(line_buff,sizeof(line_buff),fd);
	
	sscanf(line_buff,"%u",&t.pid);
	const char *q =get_items(line_buff,PROCESS_ITEM);
	sscanf(q,"%ld %ld %ld %ld",&t.utime,&t.stime,&t.cutime,&t.cstime);
	fclose(fd);
	
	return (t.utime + t.stime + t.cutime + t.cstime);
}

//获取CPU占用率
float get_proc_cpu(unsigned int pid){
 	
	unsigned long totalcputime1,totalcputime2;
	unsigned long procputime1,procputime2;
	
	totalcputime1=get_cpu_total_occupy();
	procputime1=get_cpu_proc_occupy(pid);
 
	usleep(200000);
 
	totalcputime2=get_cpu_total_occupy();
	procputime2=get_cpu_proc_occupy(pid);
	
	float pcpu = 0.0;
	if(0 != totalcputime2-totalcputime1){ 
		pcpu=100.0 * (procputime2-procputime1)/(totalcputime2-totalcputime1);
	}
	
	return pcpu;
}

void get_cpuoccupy(cpu_occupy_t *cpust)
{

	FILE *fd;
	// int n;
	char buff[256];
	cpu_occupy_t *cpu_occupy;

	cpu_occupy = cpust;
	fd = fopen("/proc/stat", "r");
	if (fd == NULL)
	{
		perror("fopen:");
		exit(0);
	}
	fgets(buff, sizeof(buff), fd);
	sscanf(buff, "%s %u %u %u %u %u %u %u", cpu_occupy->name, &cpu_occupy->user, &cpu_occupy->nice, &cpu_occupy->system, &cpu_occupy->idle, &cpu_occupy->iowait, &cpu_occupy->irq, &cpu_occupy->softirq);
	fclose(fd);
}

unsigned char get_sysCpuUsage()
{
	cpu_occupy_t cpu_stat1;
	cpu_occupy_t cpu_stat2;
	double cpu;
	unsigned char c = 0;

	get_cpuoccupy((cpu_occupy_t *)&cpu_stat1);
	sleep(1);
	//第二次获取cpu使用情况
	get_cpuoccupy((cpu_occupy_t *)&cpu_stat2);
	//计算cpu使用率
	cpu = cal_cpuoccupy((cpu_occupy_t *)&cpu_stat1, (cpu_occupy_t *)&cpu_stat2);
	c = (unsigned char)cpu;

	return c;
}