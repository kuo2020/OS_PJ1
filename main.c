#define _GNU_SOURCE
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sched.h>
#include<errno.h>
#include<unistd.h>
#include"process.h"
#include"scheduler.h"

int main(int argc, char* argv[]){
	char policy[256];
	int count;
	scanf("%s%d", policy, &count);
	process proc[count];
	for (int i = 0; i < count; ++i) scanf("%s%d%d", proc[i].name, &proc[i].t_ready, &proc[i].t_exec);

	if(strcmp(policy, "FIFO") != 0 && strcmp(policy, "RR") != 0 && strcmp(policy, "SJF") != 0 && strcmp(policy, "PSJF") != 0){
		fprintf(stderr, "Invalid policy: %s", policy);
		exit(0);
	}
	scheduling(proc, count, policy);
	exit(0);
}
