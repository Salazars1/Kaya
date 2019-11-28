/*  PHASE 3
    Written by NICK STONE AND SANTIAGO SALAZAR
    Base comments and some assitance from PROFESSOR MIKEY G 
    Finished on 
*/

/*********************************************************************************************
                            Module Comment Section



*********************************************************************************************/


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
    devregarea_t *device;
    memaddr RAMTOP;
    memaddr swapAddr;

    int causeReg;
    int missSeg;
    int missPage;
    int newFrame;

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
    missSeg = ((oldState->s_asid & GET_SEG) >> SHIFT_SEG);
    missPage = ((oldState->s_asid & GET_VPN) >> 12);

    /*Acquire mutex on the swapPool data structure*/
    SYSCALL(SYSCALL4, (int)&swapSem, 0, 0);

    /*    If missing page was from KUseg3, check if the page is still missing
            -check the KUseg3 page table entry's valid bit*/
    if (missPage >= KUSEGSIZE) {
        missPage = KUSEGSIZE - 1;
    }


    /*    If no longer missing:
            -release mutex and retur ncontrol to process
                (ie. LDST oldMem)FIXME:*/
        
    

    /*    Pick a frame to use*/

    newFrame = tableLookUp();

    /*    If frame is currently occupied
            -turn the valid bit off in the page table of current frame's occupant
            -deal with TLB cache consistency
            -write current frame's contents on nthe backing store*/
    if(swapPool[newFrame].sw_asid != -1){
        swapPool[newFrame].sw_pte -> entryLO = ((swapPool[newFrame].sw_pte -> entryLO) & (0 << 9));
        TLBCLR();
        TODO: write current frame contents onto backing storage
    }


    /*  Read missing page into selected frame
        Update the swapPool data structure
        Update missing pag's page table entry: frame number and valid bit
        Deal with TLB cache consistency*/

        device = (devregarea_t*) RAMBASEADDR;
        RAMTOP = (device->rambase) + (device->ramsize);
        swapAddr = (RAMTOP - ((16 + 3)*PAGESIZE)) + (newFrame * PAGESIZE);;

        swapPool[newFrame].sw_asid = currentProcessID;
        swapPool[newFrame].sw_segNum = missSeg;
        swapPool[newFrame].sw_pgNum = missPage;
        swapPool[newFrame].sw_pte = &(uProcs[currentProcessID - 1].uProc_pte.pteTable[missPage]);
        swapPool[newFrame].sw_pte -> entryLO = swapAddr | VALID | DIRTY;


        if(missSeg == 3){
            swapPool[newFrame].sw_pte = &(kuSeg3.pteTable[missPage]);
            swapPool[newFrame].sw_pte -> entryLO = swapAddr | VALID | DIRTY | GLOBAL;
        }


    /*Release mutex and return control to process */        
    SYSCALL(SYSCALL3, (int)&swapSem, 0, 0);

    LDST(oldState);
}

void uPgmTrpHandler(){
    /*Grab the ASID*/
    int tempasid = getENTRYHI();

    /*Function to kill the process*/

    EndProcess(tempasid);

}


/*Function to kill the process and clean the frames out of memory*/
void EndProcess(int pasid)
{

    /*P ops*/
    SYSCALL(SYSCALL4,&swapSem,0,0);
    /*I do not want to be interrupted*/

    InterruptsOnOff(FALSE);



    /*NUke the TLB*/
    TLBCLR();

    InterruptsOnOff(TRUE);

    /*V ops*/
    SYSCALL(SYSCALL3,&swapSem,0,0);
    SYSCALL(SYSCALL3,&masterSem,0,0);

    SYSCALL(SYSCALL2,0,0,0);

}


/*Sys Handler! Switch Statements*/
void uSysHandler(){
    state_t * oldState;
    int casel; 
    int ASID; 
    int * sema; 
/*Grab the old state Uh oh*/
    FIXME: oldState = 
    casel = oldState -> s_a0; 

    switch(casel){

        /*Read From Terminal */
        case 9:

        break;  
        /*Write to Terminal */
        case 10:
        
        break;  
        /*Virtual V*/
        /*Not Implementing*/
        case 11:
        
        break;  
        /*Virtual P*/
        /*Not Implementing*/
        case 12:
        
        break; 
        /*Delay*/
        /*Delay a Process for N seconds*/
        case 13:
        
        break;  
        /*Disk Put*/
        case 14:
        
        break;  
        /*DISK Get*/
        case 15:
        
        break;  
        /*Write to Printer*/
        case 16:
        
        break;  
        /*Get Time of Day*/
        case 17:
        
        break;  
        /*Terminate*/
        case 18: 
        /*Call the End process function*/
        EndProcess();
        break; 

    }

    LDST(oldState);

}


/*----------------------------------------------*/

void tableLookUp(){
    static int nextVal = 0;
    nextVal = (nextVal + 1) % SWAPPOOLSIZE;
    return (nextVal);
}

/*HELPER FUNCTIONS*/

void DISKIO(){ 


}




