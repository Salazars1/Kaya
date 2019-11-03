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

/* Variables that the scheduler uses from initial.c These are Initial global variables and extern in initial.e*/
extern int processCount;
extern int softBlockCount;
extern pcb_t *currentProcess;
extern pcb_t *readyQue;
/* Set global variables in scheduler in order to track timing that each process is running*/
cpu_t quantumrun;
cpu_t Quantumstart;
/*  Round Robin algorithm that schedules each process that it is going to be executed by the system.
    Under certain conditions, it PANICS or performs the appropiate function call.
    Parameters: None
    Return: Void
    */
void scheduler()
{
    /*If the current Process is not null Meaning that the Quantum is up*/
    if(currentProcess !=NULL){
        /*Get the time that the clock is right now (how long the process has been running)*/
        STCK(quantumrun);
        /*Set the current Process time to be the old time added to the difference the quantum started and how long the store clock value was */
        currentProcess -> p_timeProc = (currentProcess -> p_timeProc) + (quantumrun - Quantumstart);
        /*Kernel Panic when these are active but We might need these */
        /*insertProcQ(&readyQue,currentProcess);*/
        /*currentProcess = NULL;*/
    }
    /*Set a new process block pointer*/
    pcb_t * NotCurr;
    /*Remove a process from the ready queue and set it to the new pointer*/
    NotCurr = removeProcQ(&readyQue);
    /*If the removed process is Not NULL*/
    if(NotCurr != NULL){
        currentProcess = NotCurr;
        STCK(Quantumstart);
        setTIMER(QUANTUM);
        LDST(&(currentProcess -> p_s));
    }
    /*If the new process removed from the ready queue is NULL*/
    if(NotCurr == NULL)
    {
        /*Set current process to NULL (No processes ready to be run) */
        currentProcess = NULL;
        /*Check to see if we have any processes remainning */
        if (processCount == 0)
        { /* Everything finished running correctly */

            HALT();
        }
        /*Still processes that need to be run */
        if (processCount > 0)
        {
            /*WE have processes but we have no processes on the ready queue or the blocked queue
            * This is an Oh fuck moment and a deadlock case PANIC
            */
            if (softBlockCount == 0)
            { /* DEADLOCK CASE */
                PANIC();
            }
            else
            {
                /*We have processes that are blocked and we need to wait with interrupts and exceptions enabled*/
                /* Processor is twiddling its thumbs (JOBS WAITING FOR IO BUT NONE IN THE PROCESSQUEUE) */
                /*Tested*/
                setSTATUS(ALLOFF | IEON | IECON | IMON);
                WAIT();
            }
        }
    }



}


