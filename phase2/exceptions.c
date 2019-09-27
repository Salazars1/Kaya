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



extern int processCount;
extern int softBlockCount;
extern pcb_t *currentProcess;
extern pcb_t *readyQue;
extern int semD[SEMNUM];


/*  Declaration of helper fucntions. Further documentation will be provided
    in the actual method.*/
HIDDEN void Syscall1(state_t* caller);
HIDDEN void Syscall2();
HIDDEN void Syscall3(state_t* caller);
HIDDEN void Syscall4(state_t* caller);
HIDDEN void Syscall5(state_t* caller);
HIDDEN void Syscall6(state_t* caller);
HIDDEN void Syscall7(state_t* caller);
HIDDEN void Syscall8(state_t* caller);

/*Commenting the Logic of the some of the functions */
void SYSCALLHandler()
{
/*
    There are 8 System calls that our Handler must look out for 
    Of these first 8 System calls the Kernel Mode must be active
    In order for these commands to execute */

   state_t* caller;
   int sysRequest;




    state_t PTR PrevState = (state_t PTR)SYSCALLOLDAREA;

    /*The SYs call is not one of the first 8 sys calls*/
    if (PrevState->s_a0 > 8)
    {
        PassUpOrDie();
        /*Pass up Or Die */
    }

    /*The Sys call is one of the first 8 Sys Calls */
    if (PrevState->s_a0 <= 8)
    {
        if (PrevState->s_status == UMOFF) {
            /*Not Privileged Die*/
            PrgTrapHandler();
        }
        else
        {
            /*Switch Statement for the first 8 Sys calls begins */
            switch (8)
            {

            case 1:
                /*SYS CALL*/
                break;

            case 2:
/*SYS CALL*/
                break;

            case 3:
            /*SYS CALL*/
                break;

            case 4:
                /*SYS CALL*/
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
            default: 
                break; 


            }
        }
    }

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
HIDDEN void Syscall1(state_t* caller){

    pcb_t * birthedproc = allocPcb();
    
    if(emptyProcQ(birthedproc) == TRUE){ /*Check space in the ready queue to make sure we have room to allocate*/ 
        /*We did not have any more processses able to be made so we send back a -1*/
        caller -> s_v0 = -1; 
        
    }
    else { 
        processCount++; 
        /* INserts the new process into the Ready Queue*/
        insertProcQ(currentProcess, birthedproc);
        /*Makes the new process a child of the currently running process calling the sys call */
        insertChild(currentProcess, birthedproc);
        /*WE were able to allocate thus we put 0 in the v0 register*/
        caller -> s_v0 = 0; 
        /*Copy the calling state into the new processes state*/
        CtrlPlusc(caller, birthedproc->p_s);

        int SYSCALL (1, caller);
        
        

    }
    

}

HIDDEN void Syscall2(){




}

HIDDEN void Syscall3(state_t* caller){
    
}

HIDDEN void Syscall4(state_t* caller){

}

HIDDEN void Syscall5(state_t* caller){

}

HIDDEN void Syscall6(state_t* caller){

}

HIDDEN void Syscall7(state_t* caller){

}

HIDDEN void Syscall8(state_t* caller){

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
void CtrlPlusc(state_PTR OldState, state_PTR NewState){
    /*Move all of the contents from the old state into the new*/
    NewState -> s_asid = OldState -> s_asid; 
    NewState -> s_status = OldState -> s_status; 
    NewState -> s_pc = OldState -> s_pc; 
    /*Loop through all of the registers in the old state and write them into the new state*/
    int i;
    for(i = 0; i < STATEREGNUM; i++){
        NewState ->s_reg[i] = OldState ->s_reg[i];


    }




}