/*  PHASE 2
    Written by NICK STONE AND SANTIAGO SALAZAR
    Base code and Comments from PROFESSOR MIKEY G 
    Finished on  
*/

/*********************************************************************************************
                            Module Comment Section

**********************************************************************************************/
#include "../h/const.h"
#include "../h/types.h"
#include "../e/initial.e"
#include "../e/pcb.e"

/* Variables that the scheduler uses from initial.c*/
int processCount;
int softBlockCount;
pcb_t *currentProcess;
pcb_t *readyQue;

/*  Nucleus guarantees finite progress (NO STARVATION), therefore, every ready process
    will have an opportunity to execute. For simplicity’s sake this will be a simple round-robin
    scheduler with a time slice value of 5 milliseconds.
    Parameters: 
    Retrun:     */
void scheduler()
{

    /*  Process was running and either was blocked or its pointer got 
        removed from readyQue */


    if (!emptyProcQ(readyQue)) /*  Starts next process in Queue*/
    {
        currentProcess = removeProcQ(&readyQue);
    }

    else
    {
        currentProcess = NULL; /* no process is running*/

        if (processCount == 0)
        { /* Everything finished running correctly */
            HALT();
        }
        else if (sftBlkCount == 0)
        { /* DEADLOCK CASE */
            PANIC();
        }
        else
        { /* Processor is twiddling its thumbs */
            WAIT();
        }
    }
}