#include "../h/types.h"
#include "../h/const.h"
#ifndef initial
#define initial




extern int processCount;
extern int softBlockCount;
extern pcb_t *currentProcess;
extern pcb_t *readyQue;
extern int semD[SEMNUM];
extern cpu_t currentTOD;
extern cpu_t TODStart;


#endif