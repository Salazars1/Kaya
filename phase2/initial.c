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
int processCount;
int softBlockCount;
pcb_t *currentProcess;
pcb_t *readyQue;
int semD[SEMNUM];

extern void test(); 
/*LET THE OS KNOW WHERE TO START!*/
int main()
{
    devregarea_t* deviceBus;
    deviceBus = (devregarea_t*) RAMBASEADDR;
    unsigned int RAMTOP;                                         /* Defines RAMTOP as an unsigned integer*/
    RAMTOP = (deviceBus->rambase) + (deviceBus->ramsize);   /*Sets RAMTOP according to the hardware memory*/

    state_t *newLocation; /* Initialize the new Processor State Areas */

        /*  Initialize the PCB and ASL lists  */
    initPcbs();
    initASL();

    /*  Initialize phase2 global variables  */
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

    /* SYSCALL BREAK*/
    newLocation = (state_t *)SYSCALLNEWAREA;
    newLocation->s_pc = (memaddr) SYSCALLHandler;     
    newLocation->s_sp = RAMTOP;
    newLocation->s_status = ALLOFF| VMOFF | IMON | UMOFF;  /* Turns the VMOFF, IMON, UMOFF (Checks const.h for info in the names) */

    /* PROGRAM TRAP */
    newLocation = (state_t*) PRGMTRAPNEWAREA;
    newLocation->s_pc = (memaddr) PrgTrapHandler;
    newLocation->s_sp = RAMTOP;
    newLocation->s_status = ALLOFF | VMOFF | IMON | UMOFF; /* Turns the VMOFF, IMON, UMOFF (Checks const.h for info in the names) */

    /* TLB MANAGEMENT */
    newLocation = (state_t *)TBLMGMTNEWAREA;
    newLocation->s_pc = (memaddr) TLBTrapHandler;
    newLocation->s_sp = RAMTOP;
    newLocation->s_status = ALLOFF | VMOFF | IMON | UMOFF; /* Turns the VMOFF, IMON, UMOFF (Checks const.h for info in the names) */

    /* INTERRUPTS */
    newLocation = (state_t *)INTERRUPTNEWAREA;
    newLocation->s_pc = (memaddr) IOTrapHandler;
    newLocation->s_sp = RAMTOP;
    newLocation->s_status = ALLOFF | VMOFF | IMON | UMOFF; /* Turns the VMOFF, IMON, UMOFF (Checks const.h for info in the names) */

    /* Create initial process (alloc PCB)*/
    currentProcess = allocPcb();
    processCount++;           /* Adds one more process to the process count */

    /* Initialize p_s with all the requirements */
    (currentProcess->p_s).s_sp = (RAMTOP - PAGESIZE);
    (currentProcess->p_s).s_pc = (memaddr) test;
    (currentProcess->p_s).s_t9 = (memaddr) test;
    (currentProcess->p_s).s_status = ALLOFF | IEON | IMON | UMOFF; /* Turns the VMOFF, IMON, UMOFF (Checks const.h for info in the names) */

    
    insertProcQ(&readyQue, currentProcess); /* Inserts the proces into the pcb data structure */
    currentProcess = NULL;
    
    LDIT(IOCLOCK);  /*Sets the semaphore pseudoclock*/

    /* Lets the scheduler file take over.*/
   scheduler();

    return 0;
}
