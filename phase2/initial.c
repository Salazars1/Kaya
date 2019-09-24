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
#include "../e/asl.e"
#include "../e/pcb.e"
#include "../e/initial.e"
#include "../e/interrupts.e"
#include "../e/exceptions.e"
#include "../e/scheduler.e"
#include "p2test.e"

/* Global Variables*/
int processCount;
int softBlockCount;
pcb_t *currentProcess;
pcb_t *readyQue;
int semD[SEMNUM];

/*LET THE OS KNOW WHERE TO START!*/
int main()
{
    devregarea_t deviceBus = (devregarea_t*) RAMBASEADDR;
    memaddr RAMTOP; /* Defines RAMTOP as an unsigned integer*/
    RAMTOP = (deviceBus->rambase) + (deviceBus->ramsize); /*Sets RAMTOP according to the hardware memory*/

    state_t *newLocation; /* Initialize the new Processor State Areas */

    /* SYSCALL BREAK*/
    newLocation = (state_t *)SYSCALLNEWAREA;
    newLocation->s_pc = (memaddr) SYSCALLHandler();     
    newLocation->s_sp = RAMTOP;
    newLocation->s_status = ALLOFF | VMOFF | IMON | UMOFF;  /* Turns the VMOFF, IMON, UMOFF (Checks const.h for info in the names) */

    /* PROGRAM TRAP */
    newLocation = (state_t *)PRGMTRAPNEWAREA;
    newLocation->s_pc = (memaddr) PrgTrapHandler();
    newLocation->s_sp = RAMTOP;
    newLocation->s_status = ALLOFF | VMOFF | IMON | UMOFF; /* Turns the VMOFF, IMON, UMOFF (Checks const.h for info in the names) */

    /* TLB MANAGEMENT */
    newLocation = (state_t *)TBLMGMTNEWAREA;
    newLocation->s_pc = (memaddr) TBLMgmtExc();
    newLocation->s_sp = RAMTOP;
    newLocation->s_status = ALLOFF | VMOFF | IMON | UMOFF; /* Turns the VMOFF, IMON, UMOFF (Checks const.h for info in the names) */

    /* INTERRUPTS */
    newLocation = (state_t *)INTERRUPTNEWAREA;
    newLocation->s_pc = (memaddr) TLBTrapHandler();
    newLocation->s_sp = RAMTOP;
    newLocation->s_status = ALLOFF | VMOFF | IMON | UMOFF; /* Turns the VMOFF, IMON, UMOFF (Checks const.h for info in the names) */

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

    /* Create initial process (alloc PCB)*/
    pcb_t *p;
    p = allocPcb();

    /* Initialize p_s with all the requirements */
    p->p_s.s_sp = (RAMTOP - PAGESIZE);
    p->p_s.s_pc = (memaddr)test;
    p->p_s.s_t9 = (memaddr)test;
    p->p_s.s_status = ALLOFF | VMOFF | IMON | UMOFF; /* Turns the VMOFF, IMON, UMOFF (Checks const.h for info in the names) */

    processCount++;           /* Adds one more process to the process count */
    insertProQ(&readyQue, p); /* Inserts the proces into the pcb data structure */

    p = NULL;

    /* Lets the scheduler file take over.*/
    scheduler();

    return 0;
}
