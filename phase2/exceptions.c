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


/*  Declaration of helper fucntions. Further documentation will be provided
    in the actual method.*/
HIDDEN void Syscall1();
HIDDEN void Syscall2();
HIDDEN void Syscall3();
HIDDEN void Syscall4();
HIDDEN void Syscall5();
HIDDEN void Syscall6();
HIDDEN void Syscall7();
HIDDEN void Syscall8();


/*Commenting the Logic of the some of the functions */
void SYSCALLHandler(){
/**
 * There are 8 System calls that our Handler must look out for 
 * Of these first 8 System calls the Kernel Mode must be active
 * In order for these commands to execute 
 * 
*/
state_t PTR PrevState = (state_t PTR) SYSCALLOLDAREA;



if(PrevState->s_a0 > 9){
    PassUpOrDie();
/*Pass up Or Die */


}

if(PrevState -> s_a0 <= 8){
    if(PrevState -> s_status == UMOFF){
        /*Not Privileged Die*/
    }
    else{
        /*Switch Statement for the first 8 Sys calls begins */





    }
}



/**
 * If the System Call is 9 -255 Meaning that it is not one of the Sys calls that we are implementing
 * Then we are going to Pass up or Die 
 * Then we know that if the sys call numbers are values 1-8 then we need to be in kernel mode to execute 
*/









}


void PrgTrapHandler(){


}

void TLBTrapHandler(){

    
}

void PassUpOrDie(){

}
