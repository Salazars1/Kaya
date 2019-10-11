#ifndef initial
#define initial

#include "../h/types.h"

EXTERN int processCount;
EXTERN int softBlockCount;
EXTERN pcb_t *currentProcess;
EXTERN pcb_t *readyQue;
EXTERN int semD[SEMNUM];


#endif