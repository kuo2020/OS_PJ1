#ifndef _SCHEDULING_H_
#define _SCHEDULING_H_

#include"process.h"
/* Return next process to run */
int next_process(process* proc, int nproc, char* policy);

/* Running scheduler */
int scheduling(process* proc, int nproc, char* policy);

#endif
