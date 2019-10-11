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
#include "../e/pcb.e"
#include "../e/asl.e"
#include "../e/initial.e"
#include "../e/scheduler.e"
#include "../e/pcb.e"

extern int processCount;
extern int softBlockCount;
extern pcb_t *currentProcess;
extern pcb_t *readyQue;
extern int semD[SEMNUM];

/*  Declaration of helper fucntions. Further documentation will be provided
    in the actual method.*/
HIDDEN void Syscall1(state_t *caller);
HIDDEN void Syscall2();
HIDDEN void Syscall3(state_t *caller);
HIDDEN void Syscall4(state_t *caller);
HIDDEN void Syscall5(state_t *caller);
HIDDEN void Syscall6(state_t *caller);
HIDDEN void Syscall7(state_t *caller);
HIDDEN void Syscall8(state_t *caller);
HIDDEN void CtrlPlusC(state_t *oldState, state_t *newState);

/*Commenting the Logic of the some of the functions */
void SYSCALLHandler()
{FIXME:
    /*
    There are 8 System calls that our Handler must look out for 
    Of these first 8 System calls the Kernel Mode must be active
    In order for these commands to execute */

    state_t *prevState;
    state_t *pgm;
    (memaddr) prevStatus;
    int case; 

    prevState = (state_t *)SYSCALLOLDAREA; /* prevState status*/
    prevStatus = prevState->s_status;
    case = prevState->s_a0;
    /*The SYs call is not one of the first 8 sys calls*/
    if ((prevState->s_a0 > 0) && (prevState->s_a0 < 9) && (prevStatus = !ALLOFF))? ? ? ? ? ? ? ? ? ? ? how to include UMOFF ? &in the codition ? ? ? ?
        {
            PrgTrapHandler(); /*Trap Handler */
            ......            //TODO:
                ...
        }

    /*Switch statement to determine which Syscall we are about to do. If there is no case, we execute the default case */
    switch (case)
    {

    case 1: /* SYSCALL 1 (BIRTH) ITS ABOUT TO BE CALLED)*/
        Syscall1(currentProcess);
        break;

    case 2:
        /*SYS CALL*/
        Syscall2();
        break;

    case 3:
        /*SYS CALL*/
        Syscall3(prevState);
        break;

    case 4:
        /*SYS CALL*/
        Syscall4(prevState);
        break;

    case 5:
        /*SYS CALL*/
        Syscall5(prevState);
        break;

    case 6:
        /*SYS CALL*/
        break;

    case 7:
        /*SYS CALL*/
        break;

    case 8:
        Syscall8(prevState);
        break;
    }

    /*We should NEVER GET HERE. IF WE DO, WE DIE*/

    /**
 * If the System Call is 9 -255 Meaning that it is not one of the Sys calls that we are implementing
 * Then we are going to Pass up or Die 
 * Then we know that if the sys call numbers are values 1-8 then we need to be in kernel mode to execute 
*/
}

void PrgTrapHandler()
{
    /*Call Pass Up Or Die*/
    PassUpOrDie();
}

void TLBTrapHandler()
{
      /*Call Pass Up Or Die*/
    PassUpOrDie();
}
/*
WE WAIT 

*/

void PassUpOrDie()
{
    state_PTR Location; 
    state_PTR state; 


}

HIDDEN void Syscall1(state_t *caller)
{

    pcb_t *birthedProc = allocPcb();

    if (emptyProcQ(birthedProc) == TRUE)
    { /*Check space in the ready queue to make sure we have room to allocate*/
        /*We did not have any more processses able to be made so we send back a -1*/
        caller->s_v0 = -1;
    }
    else
    {
        processCount++;

        /*Makes the new process a child of the currently running process calling the sys call */
        insertChild(currentProcess, birthedProc);

        /* Inserts the new process into the Ready Queue*/
        insertProcQ(currentProcess, birthedProc);

        /*WE were able to allocate thus we put 0 in the v0 register*/
        caller->s_v0 = 0;

        /*Copy the calling state into the new processes state*/
        CtrlPlusc(caller, birthedProc->p_s);

        LoadState(caller);
    }
}

/*FIXME:*/
HIDDEN void Syscall2()
{
    pcb_t *temp = currentProcess;

    while (temp->p_child != NULL)
    {
        temp = temp->p_child;
    }
    /*We are at the bottom Most child
      Kill every child in list. Rinse and Repeat
    
    */
    pcb_t *temp2;
    while (temp->p_next != NULL)
    {
        temp2 = temp->p_next;
        removeChild(temp);
        freePcb(temp);
        temp = temp2;
        processCount--;
    }

    if (currentProcess->p_child != NULL)
    {
        /*You still got kids? Ill fucking do it again*/
        Syscall2();
    }
    else
    {
        /*Batter up mother fucker its time to die */

        /*WE have one less process to worry about 
    We remove the process from the ready queue 
    We then free the pcb now we can allocate a new pcb 
   
   */
        processCount--;
        removeProcQ(currentProcess);
        freePcb(currentProcess);
    }
}

HIDDEN void Syscall3(state_t *caller)
{
    pcb_t *newProccess = NULL;
    int *semV = (int *)caller->s_a1;
    semV++; /* increment semaphore  */

    if ((*semV) <= 0)
    { /* waiting in the semaphore */
        newProccess = removeBlocked(semV);
        if (newProccess != NULL)
        { /* add it to the ready queue */
            insertProcQ(&readyQueue, newProccess);
        }
    }

    LoadState(caller); /* returns control to caller */
}

HIDDEN void Syscall4(state_t *caller)
{
    int *semV = (int *)caller->s_a1;
    semV--; /* decrement semaphore */
    if ((*semV) < 0)
    { /* there is something controlling the semaphore */
        CtrlPlusC(caller, &(currentProcess->p_s));
        insertBlocked(semV, currentProcess);
        scheduler();
    }
    /* nothing had control of the sem, return control to caller */
    LoadState(caller);
}

HIDDEN void Syscall5(state_t *caller)
{
    
    if (caller->s_a1 == 0)
    { /*TLB TRAP*/
        if (currentProcess->newTLB != NULL)
        {
            syscall2(); /* already called sys5 */
        }
        /* assign exception values */
        currentProcess->newTLB = (state_t *)caller->s_a3;
        currentProcess->oldTLB = (state_t *)caller->s_a2;
    }

    if (caller->s_a1 == 1)
    {
        if (currentProcess->newProgramTrap != NULL)
        {
            syscall2(); /* already called sys5 */
        }
        /* assign exception values */
        currentProcess->newProgramTrap = (state_t *)caller->s_a3;
        currentProcess->oldProgramTrap = (state_t *)caller->s_a2;
    }

    else
    {
        if (currentProcess->newSys != NULL)
        {
            syscall2(); /* already called sys5 */
        }
        /* assign exception values */
        currentProcess->newSys = (state_t *)caller->s_a3;
        currentProcess->oldSys = (state_t *)caller->s_a2;
    }

    LoadState(caller);
}
/*TODO:*/
HIDDEN void Syscall6(state_t *caller)
{
}
/*TODO:*/
HIDDEN void Syscall7(state_t *caller)
{
}

HIDDEN void Syscall8(state_t *caller)
{
    int intlNo;
    int dnum;
    int waitforTermRead;

    int index;
    int *sem;

    lineNo = caller->s_a1;
    dnum = caller->s_a2;
    waitforTermRead = caller->s_a3; /* terminal read  or write */

    /* what device is going to be computed*/
    if (lineNo == TERMINT && waitforTermRead == TRUE)
    {
        /* terminal read */
        index = DEVPERINT * (lineNo - DEVWOSEM + waitforTermRead) + dnum;
    }
    else
    {
        /* anything else */
        index = DEVPERINT * (lineNo - DEVWOSEM) + dnum;
    }

    semD[index]--;
    

    if (semD[index] < 0)
    {

        insertBlocked(sem, currentProcess);
        CtrlPlusC(caller, &(currentProcess->p_s));
        sftBlkCount++;

        /*DECIDED TO CALL SCHEDULER instead of giving back time to the process that was interrupted
        Keeps the overall flow of the program and since there is no starvation, eventually that process
        will get its turn to play with the processor*/
        scheduler();
    }

}

/*This state will copy all of the contents of the old state into the new state*/
void CtrlPlusC(state_t *oldState, state_t *newState)
{
    /*Move all of the contents from the old state into the new*/
    NewState->s_asid = OldState->s_asid;
    NewState->s_status = OldState->s_status;
    NewState->s_pc = OldState->s_pc;
    /*Loop through all of the registers in the old state and write them into the new state*/
    int i;
    for (i = 0; i < STATEREGNUM; i++)
    {
        NewState->s_reg[i] = OldState->s_reg[i];
    }
}

/*Function that is designed for ME to be able to read that LDST is Load State*/

void LoadState(state_PTR s){LDST(s);}