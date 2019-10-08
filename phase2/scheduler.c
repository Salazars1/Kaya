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

/* Variables for maintaining CPU time*/
cpu_t currentTOD;
cpu_t TODStart;


/* Variables that the scheduler uses from initial.c*/
int processCount;
int softBlockCount;
pcb_t *currentProcess;
pcb_t *readyQueue;

/*  Nucleus guarantees finite progress (NO STARVATION), therefore, every ready process
    will have an opportunity to execute. For simplicity’s sake this will be a simple round-robin
    scheduler with a time slice value of 5 milliseconds.
    Parameters: 
    Retrun:     */
void scheduler()
{

    /*  Process was running and either was blocked or its pointer got 
        removed from readyQue */

    STCK(currentTOD);        /* Get start time */
    currentProcess->cpu_time = (currentProcess->cpu_time) + (currentTOD - TODStart); /* save how much time current process used on CPU */

    if (!emptyProcQ(readyQueue)) /*  Starts next process in Queue*/
    {
        currentProcess = removeProcQ(&readyQueue);    /* Remove process from Queue */
        STCK(TODStart);        /* Get start time */

        SetTIMER (QUANTUM);

        LDST(&(currentProcess -> p_s));
    }
    else
    {   /* There is nothing on the ReadyQueue */
        
        currentProcess = NULL; /* no process is running*/

        if (processCount == 0)
        { /* Everything finished running correctly */
            HALT();
        }
        else if (softBlockCount == 0)
        { /* DEADLOCK CASE */
            PANIC();
        }
        else 
        { /* Processor is twiddling its thumbs (JOBS WAITING FOR IO BUT NONE IN THE PROCESSQUEUE) */
            
            SetSTATUS(ALLOFF|IEON|IECON|IMON);
            WAIT();
        }
    }
}