/*  PHASE 3
    Written by NICK STONE AND SANTIAGO SALAZAR
    Base comments and some assitance from PROFESSOR MIKEY G 
    Finished on 
*/

#include "../h/const.h"
#include "../h/types.h"

#include "../e/vmIOsupport.e"
#include "../e/initProc.e"

#include "/usr/local/include/umps2/umps/libumps.e"

void pager()
{
    /*TLB Handler Outline:*/
    int currentProcessID;
    state_t* oldState;
    int causeReg;

    /*Who am I?
        The current processID is in the ASID regsiter
        This is needed as the index into the phase 3 global structure*/
    currentProcessID = (int)((getENTRYHI() & GETASID) >> 6);
    oldState = (state_t*) &(uProcs[currentProcessID-1].UProc_OldTrap[TLBTRAP]);

    /*Why are we here?
        Examine the oldMem Cause register*/
    causeReg = (oldState->s_cause);

    /*If TLB invalid (load or store) continue; o.w. nuke them*/
    if((currentProcessID!=TLBLOAD) || (currentProcessID!=TLBSTORE)){
        SYSCALL(SYSCALL2,0,0,0);
    }
    /*Which page is missing?
        oldMem ASID register has segment no and page no*/


    /*Acquire mutex on the swapPool data structure*/
    SYSCALL(SYSCALL4, (int)&swapSem, 0, 0);

    /*    If missing page was from KUseg3, check if the page is still missing
            -check the KUseg3 page table entry's valid bit*/

    /*    If no longer missing:
            -release mutex and retur ncontrol to process
                (ie. LDST oldMem)*/
        
    /*    Pick a frame to use*/

    /*    If frame is currently occupied
            -turn the valid bit off in the page table of current frame's occupant
            -deal with TLB cache consistency
            -write current frame's contents on nthe backing store*/


    /*  Read missing page into selected frame
        Update the swapPool data structure
        Update missing pag's page table entry: frame number and valid bit
        Deal with TLB cache consistency*/

    /*Release mutex and return control to process */        
    SYSCALL(SYSCALL3, (int)&swapSem, 0, 0);
}

void uPgmTrpHandler(){
    
}

void uSysHandler(){

}




