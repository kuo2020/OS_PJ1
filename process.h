#ifndef _PROCESS_H_
#define _PROCESS_H_

#include<sys/types.h>

/* Running one unit time */
#define UNIT_T()				\
{						\
	volatile unsigned long i;		\
	for (i = 0; i < 1000000UL; i++);	\
}						\

typedef struct{
	char name[32];
	int t_ready;
	int t_exec;
	pid_t pid;
}process;

/* Assign process to specific core */
int assign_cpu(int pid, int core);

/* Execute the process and return pid */
int exec(process proc);

/* Set very low priority tp process */
int block(int pid);

/* Set high priority to process */
int wakeup(int pid);

#endif
