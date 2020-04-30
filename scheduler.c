#define _GNU_SOURCE
#include"process.h"
#include"scheduler.h"
#include<stdlib.h>
#include<string.h>
#include<signal.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sched.h>

static int t_last, ntime, running, finish_cnt;

int cmp(const void *a, const void *b){
	return ((process *)a)->t_ready - ((process *)b)->t_ready;
}

int next_process(process* proc, int count, char* policy){
	if(running != -1 && (strcmp(policy, "SJF") == 0 || strcmp(policy, "FIFO") == 0))
		return running;
	int ret = -1;

	if(strcmp(policy, "FIFO") == 0){
		for(int i = 0; i < count; ++i){
			if(proc[i].pid == -1 || proc[i].t_exec == 0)
				continue;
			if(ret == -1 || proc[i].t_ready < proc[ret].t_ready)
				ret = i;
		}
    }
	else if(strcmp(policy, "RR") == 0){
		if(running == -1){
			ret = -1;
			for (int i = 0; i < count; ++i) {
				if(proc[i].pid != -1 && proc[i].t_exec > 0){
					if(ret == -1){
						ret = i;
						continue;
					}
					if(proc[i].t_ready < proc[ret].t_ready) ret = i;
					else if(proc[i].t_ready == proc[ret].t_ready)
						if(i < ret) ret = i;
				}
			}
		}
		else if((ntime - t_last) % 500 == 0){
			proc[running].t_ready = ntime;
			/*
			ret = (running + 1) % count;
			while(proc[ret].t_ready > ntime || proc[ret].t_exec == 0)	ret = (ret + 1) % count;
			*/
			ret = -1;
			//printf("ntime = %d, running = %s\n", ntime, proc[running].name);
			for(int i = 0; i < count; ++i){
				//printf("%s %d %d\n", proc[i].name, proc[i].t_ready, proc[i].t_exec);
				if(proc[i].pid == -1 || proc[i].t_exec == 0) continue;
				if(ret == -1){
					ret = i;
					continue;
				}
				if(proc[i].t_ready < proc[ret].t_ready) ret = i;
				else if(proc[i].t_ready == proc[ret].t_ready){
					if(i < ret) ret = i;
				}
			}
		}
		else ret = running;
	}
	else if(strcmp(policy, "PSJF") == 0 || strcmp(policy, "SJF") == 0){
		for (int i = 0; i < count; ++i) {
			if(proc[i].pid == -1 || proc[i].t_exec == 0)
				continue;
			if(ret == -1 || proc[i].t_exec < proc[ret].t_exec)
				ret = i;
		}
	}
	return ret;
}

int scheduling(process* proc, int count, char* policy){
	qsort(proc, count, sizeof(process), cmp);
	for (int i = 0; i < count; ++i)	proc[i].pid = -1;

	assign_cpu(getpid(), 0);
	wakeup(getpid());
	
	ntime = 0;
	running = -1;
	finish_cnt = 0;
	
	while(1){
		if(running != -1 && proc[running].t_exec == 0){	
			waitpid(proc[running].pid, NULL, WUNTRACED);
			printf("%s %d\n", proc[running].name, proc[running].pid);
			running = -1;
			finish_cnt++;
			if (finish_cnt == count)
				break;
		}

		for (int i = 0; i < count; ++i){
			if (proc[i].t_ready == ntime){
				proc[i].pid = exec(proc[i]);
				block(proc[i].pid);
			}
		}

		int next = next_process(proc, count, policy);
		if(next != -1 && running != next){
			wakeup(proc[next].pid);
			block(proc[running].pid);
			running = next;
			t_last = ntime;
		}
		UNIT_T();
		if (running != -1)
			proc[running].t_exec--;
		ntime++;
	}
	return 0;
}
