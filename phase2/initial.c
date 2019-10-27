/*  PHASE 2
    Written by NICK STONE AND SANTIAGO SALAZAR
    Base comments and some assitance from PROFESSOR MIKEY G 
    Finished on  
*/

/*********************************************************************************************
                            Module Comment Section
    Every program needs a main. This module is the entry point of our Kaya Implementation 
    Operative System. This module populates new areas in the ROM Reserved Frame, initializes
    phase 1 code, as well as nucleus maintained variables, the nucleus maintained semaphores,
    instantes a single process in the Ready Queue, and calls the scheduler.
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





/* Global Variables*/
/*The number of processes on the ready QUeue*/
int processCount;
/*The number of processes on the blocked queue */
int softBlockCount;
/*Pointer to the current running process*/
pcb_t *currentProcess;
/*Pointer to the */
pcb_t *readyQue;
/*49 Synchronization Semaphores in a list*/
int semD[SEMNUM];

/*Import the test function from the test file */
extern void test();
/*LET THE OS KNOW WHERE TO START!*/
int main()
{
    /*INitialize PCV and ASL */
    initPcbs();
    initASL();
    /*Set the global variables to the starting values... 0,0,NULL, NULL*/
    processCount = 0;
    softBlockCount = 0;
    currentProcess = NULL;
    readyQue = mkEmptyProcQ();
  /* iniltialize semaphores to 0*/
    int i;
    for (i = 0; i < SEMNUM; i++)
    {
        semD[i] = 0;
    }

    /*Allocate a process to be set as the current Process and increment process count*/
    currentProcess = allocPcb();
    processCount = processCount + 1;   
    /*Define the Device bus and set the value of the RAMTOP*/
    devregarea_t* deviceBus;
    deviceBus = (devregarea_t*) RAMBASEADDR;
    unsigned int RAMTOP;                                         /* Defines RAMTOP as an unsigned integer*/
    RAMTOP = (deviceBus->rambase) + (deviceBus->ramsize);   /*Sets RAMTOP according to the hardware memory*/

    state_t* newLocation; /* Initialize the new Processor State Areas */
    /* SYSCALL BREAK*/
    newLocation = (state_t*) SYSCALLNEWAREA;
    newLocation->s_pc = (memaddr) SYSCALLHandler;     
    newLocation->s_t9 = (memaddr) SYSCALLHandler;
    newLocation->s_sp = RAMTOP;
    newLocation->s_status = ALLOFF;  /* Turns the VMOFF, IMON, UMOFF (Checks const.h for info in the names) */

    /* PROGRAM TRAP */
    newLocation = (state_t*) PRGMTRAPNEWAREA;
    newLocation->s_pc = (memaddr) PrgTrapHandler;
    newLocation->s_t9 = (memaddr) PrgTrapHandler;
    newLocation->s_sp = RAMTOP;
    newLocation->s_status = ALLOFF; /* Turns the VMOFF, IMON, UMOFF (Checks const.h for info in the names) */

    /* TLB MANAGEMENT */
    newLocation = (state_t*) TLBMGMTNEWAREA;
    newLocation->s_pc = (memaddr) TLBTrapHandler;
    newLocation->s_t9 = (memaddr) TLBTrapHandler;
    newLocation->s_sp = RAMTOP;
    newLocation->s_status = ALLOFF; /* Turns the VMOFF, IMON, UMOFF (Checks const.h for info in the names) */
    /* INTERRUPTS */
    newLocation = (state_t*) INTERRUPTNEWAREA;
    newLocation->s_pc = (memaddr) IOTrapHandler;
    newLocation->s_t9 = (memaddr) IOTrapHandler;
    newLocation->s_sp = RAMTOP;
    newLocation->s_status = ALLOFF; /* Turns the VMOFF, IMON, UMOFF (Checks const.h for info in the names) */
    /* Initialize p_s with all the requirements */
    currentProcess->p_s.s_sp = (RAMTOP - PAGESIZE);
    currentProcess->p_s.s_pc = (memaddr) test;
    currentProcess->p_s.s_t9 = (memaddr) test;
    currentProcess->p_s.s_status = ALLOFF | IEON | IMON | TEBITON; /* Turns the VMOFF, IMON, UMOFF (Checks const.h for info in the names) */

    /*Insert the current process onto the ready Queue (Not Blocked)*/    
    insertProcQ(&readyQue, currentProcess); /* Inserts the process into the pcb data structure */
    /*Since process is now on the ready queue Current Process is NULL*/
    currentProcess = NULL;
    /*Load 100 Milliseconds onto the psueodo clock */
    LDIT(IOCLOCK);  /*Sets the semaphore pseudoclock*/

    /* Lets the scheduler file take over.*/
    scheduler();

    return 0;
}




