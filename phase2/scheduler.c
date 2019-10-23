/*  PHASE 2
    Written by NICK STONE AND SANTIAGO SALAZAR
    Base comments and some assitance from PROFESSOR MIKEY G 
    Finished on  
*/

/*********************************************************************************************
                            Module Comment Section
    Nucleus guarantees finite progress (NO STARVATION), therefore, every ready process will
    have an opportunity to execute. For simplicity’s sake this will be a simple round-robin
    scheduler with a time slice value of 5 milliseconds. Also performs some simple deadlock
    detention and executes the correct procedure. 
**********************************************************************************************/
#include "../h/const.h"
#include "../h/types.h"
#include "../e/asl.e"
#include "../e/pcb.e"
#include "../e/initial.e"
#include "../e/interrupts.e"
#include "../e/exceptions.e"
#include "../e/scheduler.e"

#include "/usr/local/include/umps2/umps/libumps.e"

/* Variables for maintaining CPU time*/
cpu_t currentTOD;
cpu_t TODStart;

/* Variables that the scheduler uses from initial.c*/
extern int processCount;
extern int softBlockCount;
extern pcb_t *currentProcess;
extern pcb_t *readyQueue;


int debugthisfuckingshit(int b);

/*  Round Robin algorithm that schedules each process that it is going to be executed by the system.
    Under certain conditions, it PANICS or performs the appropiate function call. 
    Parameters: None
    Return: Void
    */
void scheduler()
{
    /*addokbuf("\n WE ARE AT THE BEGGINING OF SCHEDULER");*/
    if(currentProcess == NULL){
        addokbuf("We fucked it");
    }
    else{
    STCK(currentTOD);
    currentProcess -> p_timeProc = currentProcess -> p_timeProc + (currentTOD - TODStart);
    }
    if (!emptyProcQ(readyQue))
    { /*  Starts next process in Queue*/
        STCK(currentTOD);
        /*currentProcess -> p_timeProc = currentProcess -> p_timeProc + (currentTOD -TODStart);*/
        currentProcess = removeProcQ(&(readyQue)); /* Remove process from Queue */
        STCK(TODStart);                            /* Gets start time */

        setTIMER(QUANTUM); /* Defines Quantum to 5 ms */
        LDST(&(currentProcess->p_s));
        
    }
    else
    { /* There is nothing on the ReadyQueue */

        currentProcess = NULL; /* no process is running*/
        if (processCount == 0)
        { /* Everything finished running correctly */
            debugthisfuckingshit(4);
            HALT();
        }
        
        if (processCount > 0)
        {
            /*addokbuf("Process count is greater than 0 meaning that we have processes to run\n");*/
            if (softBlockCount == 0)
            { /* DEADLOCK CASE */
                debugthisfuckingshit(5);
                PANIC();
            }
            else
            {
                /* Processor is twiddling its thumbs (JOBS WAITING FOR IO BUT NONE IN THE PROCESSQUEUE) */
                /*Tested*/
                debugthisfuckingshit(2);
                
                setTIMER(MAXINT);
                setSTATUS(getSTATUS()|ALLOFF | IEON | IECON | IMON);
                
                WAIT();
            }
        }
    }
}
int debugthisfuckingshit(int b)
{

    return b;
}
