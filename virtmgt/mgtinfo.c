/***************************************************************
*    @file:        statusinfo.c
*
*    @brief:        从linux系统获取cpu及内存使用情况
*
*    @version    1.0
*
***************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

typedef struct        //定义一个cpu occupy的结构体
{
	char name[20];      //定义一个char类型的数组名name有20个元素
	long user; //定义一个无符号的int类型的user
	long nice; //定义一个无符号的int类型的nice
	long system;//定义一个无符号的int类型的system
	long idle; //定义一个无符号的int类型的idle
}CPU_OCCUPY;

typedef struct        //定义一个mem occupy的结构体
{
	char name[20];      //定义一个char类型的数组名name有20个元素
	long total; 
	//char name2[20];
	char unit[20];
	long free;
	long buffers;  
	long cached;                        
}MEM_OCCUPY;

typedef struct        //定义一个cpu occupy的结构体
{
	char name[20];      //定义一个char类型的数组名name有20个元素
	long rb, rpkt, r_err, r_drop, r_fifo, r_frame, r_compr, r_mcast; 
	long tb, tpkt, t_err, t_drop, t_fifo, t_coll, t_carrier, t_compr; 
}NET_OCCUPY;

int get_memoccupy (MEM_OCCUPY *mem) //对无类型get函数含有一个形参结构体类弄的指针O
{
    FILE *fd;          
    int n;             
    char buf[256];   
    MEM_OCCUPY *m;
    m=mem;
	 long MemFree, Buffers, Cached;
                                                                                                              
    fd = fopen ("/proc/meminfo", "r"); 
      
    fgets (buf, sizeof(buf), fd); 
    sscanf (buf, "%s %ld %s", &m->name, &m->total, &m->unit); 
    fgets (buf, sizeof(buf), fd); 
    sscanf (buf, "%s %ld %s", m->name, &m->free, m->unit); 
    fgets (buf, sizeof(buf), fd); 
    sscanf (buf, "%s %ld %s", m->name, &m->buffers, m->unit); 
    fgets (buf, sizeof(buf), fd); 
    sscanf (buf, "%s %ld %s", m->name, &m->cached, m->unit); 
    fclose(fd);     //关闭文件fd
    return 0;
}

int get_cpuoccupy (CPU_OCCUPY *cpust) //对无类型get函数含有一个形参结构体类弄的指针O
{   
    FILE *fd;         
    int n;            
    char buf[256]; 
    CPU_OCCUPY *cpu_occupy;
    cpu_occupy=cpust;
         
	 if ((fd = fopen ("/proc/stat", "r")) != NULL){ 
			while (fgets (buf, sizeof(buf), fd)){ 
				if( *buf == 'c' && *(buf + 1) == 'p') break; 
	        	} 
		   fclose (fd); 
	  } 
    else   
         printf("read file failed\n");                                                                                             
    
    sscanf (buf, "cpu %ld %ld %ld %ld", &cpu_occupy->user, &cpu_occupy->nice,&cpu_occupy->system, &cpu_occupy->idle);
	 //printf("%ld\n", cpu_occupy->user);
 	 return 0;    
}

int get_netoccupy (NET_OCCUPY *net) //对无类型get函数含有一个形参结构体类弄的指针O
{
    FILE *fd;                    
    char buf[256];   
    NET_OCCUPY *net_occupy;
    net_occupy = net;
	 //long MemFree, Buffers, Cached;
                                                                                                              
    fd = fopen ("/proc/net/dev", "r"); 
      
    fgets (buf, sizeof(buf), fd); 
    fgets (buf, sizeof(buf), fd); 
    fgets (buf, sizeof(buf), fd); 
    fgets (buf, sizeof(buf), fd); 
    //printf("%s",buf);
    sscanf (buf, "%s %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld", &net_occupy->name, &net_occupy->rb ,&net_occupy->rpkt, &net_occupy->r_err, &net_occupy->r_drop, &net_occupy->r_fifo ,&net_occupy->r_frame, &net_occupy->r_compr, &net_occupy->r_mcast, &net_occupy->tb, &net_occupy->tpkt);
    fclose(fd);     //关闭文件fd
    return 0;
}

int cal_cpuoccupy (CPU_OCCUPY *o, CPU_OCCUPY *n) 
{   
    unsigned long od, nd;    
    unsigned long id, sd;
    int cpu_use = 0;   
    
    od = (unsigned long) (o->user + o->nice + o->system +o->idle);//第一次(用户+优先级+系统+空闲)的时间再赋给od
    nd = (unsigned long) (n->user + n->nice + n->system +n->idle);//第二次(用户+优先级+系统+空闲)的时间再赋给od
      
    id = (unsigned long) (n->user - o->user);    //用户第一次和第二次的时间之差再赋给id
    sd = (unsigned long) (n->system - o->system);//系统第一次和第二次的时间之差再赋给sd
    if((nd-od) != 0)
    cpu_use = (int)((sd+id)*10000)/(nd-od); //((用户+系统)乖100)除(第一次和第二次的时间差)再赋给g_cpu_used
    else cpu_use = 0;
    return cpu_use;
}

int main()
{
    CPU_OCCUPY cpu_stat;
    MEM_OCCUPY mem_stat;
    NET_OCCUPY net_stat;
    long cpu_use, cpu_total;
    
    //获取内存
    get_memoccupy ((MEM_OCCUPY *)&mem_stat);
    //第一次获取cpu使用情况
    get_cpuoccupy((CPU_OCCUPY *)&cpu_stat);
	 cpu_use = cpu_stat.user + cpu_stat.nice + cpu_stat.system;
 	 cpu_total=cpu_stat.user + cpu_stat.nice + cpu_stat.system + cpu_stat.idle;
    //printf("%ld,%ld,%ld,%ld\n",cpu_stat.user,cpu_stat.nice,cpu_stat.system,cpu_stat.idle);
 	 get_netoccupy((NET_OCCUPY *)&net_stat);
    printf("%ld,%ld,%ld,%ld,%ld,%ld\n",mem_stat.total/1024,(mem_stat.total-mem_stat.free-mem_stat.buffers-mem_stat.cached)/1024,cpu_use,cpu_total,net_stat.rb,net_stat.tb);
    
    //计算cpu使用率
    //cpu = cal_cpuoccupy ((CPU_OCCUPY *)&cpu_stat1, (CPU_OCCUPY *)&cpu_stat2);
    
    return 0;
} 
