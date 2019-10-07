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
{
    /*
    There are 8 System calls that our Handler must look out for 
    Of these first 8 System calls the Kernel Mode must be active
    In order for these commands to execute */

    state_t *prevState;
    state_t *pgm;
    (memaddr) prevStatus;

    prevState = (state_t *)SYSCALLOLDAREA; /* prevState status*/
    prevStatus = prevState->s_status;

    /*The SYs call is not one of the first 8 sys calls*/
    if ((prevState->s_a0 > 0) && (prevState->s_a0 < 9) && (prevStatus = !ALLOFF))??????????? how to include UMOFF ? &in the codition ? ? ? ?
        {
            PrgTrapHandler(); /*Trap Handler */
            ......            //YET TO BE CODED STUFF
                ...
        }

    /*Switch statement to determine which Syscall we are about to do. If there is no case, we execute the default case */
    switch (prevState->s_a0)
    {

    case 1: /* SYSCALL 1 (BIRTH) ITS ABOUT TO BE CALLED)*/ ???????????? DO I NEED TO DECLARE THE STATE IN CONSTANTS.H?????
        Syscall1(currentProcess);
        break;

    case 2:
        /*SYS CALL*/ 
        Syscall2();
        break;

    case 3:
        /*SYS CALL*/
        Syscall3(prevState->s_a0);
        break;

    case 4:
        /*SYS CALL*/
        Syscall4(prevState->s_a0)
        break;

    case 5:
        /*SYS CALL*/
        break;

    case 6:
        /*SYS CALL*/
        break;

    case 7:
        /*SYS CALL*/
        break;

    case 8:
        /*SYS CALL*/
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
}

void TLBTrapHandler()
{
}
/*
WE WAIT 

*/

void PassUpOrDie()
{
}

/**/
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

        LDST(caller);
    }
}

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
    pcb_t* newProc = NULL;
    int* semV = (int*) caller->s_a1;
	(*semV)++;              /* increment semaphore  */
	
    
    if((*semV) <= 0) {      /* waiting in the semaphore */
		
		newProc = removeBlocked(semV);
		
        if(newProc != NULL){     /* add it to the ready queue */
			insertProcQ(&readyQueue, newProc);
		}
	}
	
	LDST(caller);   /* returns control to caller */
}

HIDDEN void Syscall4(state_t *caller)
{
    int* semV = (int*) caller->s_a1;
	(*semV)--;              /* decrement semaphore */
	if((*semV) < 0){        /* there is something controlling the semaphore */
		CtrlPlusC(caller, &(currProc -> p_s));
		insertBlocked(semV, currProc);
		scheduler();
	}
	/* nothing had control of the sem, return control to caller */
	LDST(caller);
}

HIDDEN void Syscall5(state_t *caller)
{
}

HIDDEN void Syscall6(state_t *caller)
{
}

HIDDEN void Syscall7(state_t *caller)
{
}

HIDDEN void Syscall8(state_t *caller)
{
}

/**
 * unsigned int	s_asid;
	unsigned int	s_cause;
	unsigned int	s_status;
	unsigned int 	s_pc;
	int	 			s_reg[STATEREGNUM];
 * 
 * 
 * 
 * 
*/

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