#define _GNU_SOURCE
#include"process.h"
#include<sched.h>
#include<errno.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/syscall.h>
#include<signal.h>
#define GET_TIME 334
#define PRINTK 333

int assign_cpu(int pid, int core){
	if(core > sizeof(cpu_set_t)){
		fprintf(stderr, "Core index error.");
		return -1;
	}
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(core, &mask);		
	if(sched_setaffinity(pid, sizeof(mask), &mask) < 0){
		//perror("sched_setaffinity");
		exit(1);
	}
	return 0;
}

int exec(process proc){
	//printf("exec\n");
	int pid = fork();
	//printf("pid=%d\n", pid);
	if(pid < 0){
		perror("fork");
		return -1;
	}
	if(pid == 0){
		//printf("pid==0\n");
		
		usleep(500);
		long start_t, end_t;
		start_t = syscall(GET_TIME);
		for(int i = 0; i < proc.t_exec; ++i)
			UNIT_T();
		end_t = syscall(GET_TIME);
    	//fprintf(stderr, "[Project1] %d %ld.%09ld %ld.%09ld\n", oldpid, start_t / BASE, start_t % BASE, end_t / BASE, end_t % BASE);
		syscall(PRINTK, getpid(), start_t, end_t);
		//long runtime = (end_t - start_t);
		//printf("%ld.%9ld\n", runtime / 1000000000, runtime % 1000000000);
		
		exit(0);
	}
	assign_cpu(pid, 1);
	return pid;
}

int block(int pid){
	struct sched_param param;	
	/* SCHED_IDLE should set priority to 0 */
	param.sched_priority = 0;
	int ret = sched_setscheduler(pid, SCHED_IDLE, &param);	
	if(ret < 0){
		//perror("sched_setscheduler");
		return -1;
	}
	kill(pid, SIGTSTP);
	//kill(pid, SIGCONT);
	return ret;
}

int wakeup(int pid){
	struct sched_param param;
	param.sched_priority = 0;
	int ret = sched_setscheduler(pid, SCHED_OTHER, &param);	
	if(ret < 0){
		//perror("sched_setscheduler");
		return -1;
	}
	kill(pid, SIGCONT);
	//kill(pid, SIGTSTP);
	return ret;
}
