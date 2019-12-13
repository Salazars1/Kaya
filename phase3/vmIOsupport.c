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


HIDDEN void Endproc(int asid);
HIDDEN void writeTerminal(char* vAddr, int len, int asid);
HIDDEN void readTerminal(char* addr, int procID);
HIDDEN void MakeTheDiskMyBitch(int block, int sector, int disk, int readWrite, memaddr addr);


/*THe following functions are testing functions for the function pager*/
void debugPager(int a){}
void debugSys(int a){}
void debugProg(int a){}


/*Debugging Pager!*/

void debugPager2(int a){}
void finegrain(int v){}


void pager()
{
    debugPager2(1);
    /*TLB Handler Outline:*/
    int currentProcessID;
    
    state_t* oldState;
    devregarea_t* device;
    memaddr thisramtop;
    memaddr swapAddr;
    /*setSTATUS( ALLOFF | IEON | IMON | TEBITON | UMOFF | VMON2);*/
    int causeReg;
    int missSeg;
    int missPage;
    int newFrame;
    int currentPage;
    unsigned int base; 
    unsigned int size; 

    int currentASID;
    debugPager2(10);
        newFrame = tableLookUp(); 
        device = RAMBASEADDR;
        
        
        base = device ->rambase; 
        size = device -> ramsize; 
        thisramtop = base + size; 
    
        /*thisramtop = (memaddr) ((device->rambase) + (device->ramsize));*/
       
        swapAddr = (memaddr)(thisramtop - ((16 + 3)*PAGESIZE)) + (newFrame * PAGESIZE);
        

    debugPager2(11);
    /*Who am I?
        The current processID is in the ASID regsiter
        This is needed as the index into the phase 3 global structure*/
    currentProcessID = (int)((getENTRYHI() & GETASID) >> 6);
    oldState = (state_t*) &(uProcs[currentProcessID-1].UProc_OldTrap[TLBTRAP]);
    int checkthisid;
    checkthisid = currentProcessID << 1; 

    /*Why are we here?
        Examine the oldMem Cause register*/
    
    causeReg = (oldState->s_cause);
    debugPager2(checkthisid);
  

    if((checkthisid < 2) || (checkthisid > 3)){

        /*SEnd there asses to the shadow realm */
        SYSCALL(SYSCALL2, 0,0,0);
    }
    
    
    /*If TLB invalid (load or store) continue; o.w. nuke them
    if((checkthisid != 2) || (checkthisid!= 3)){
        debugPager2(4);
        SYSCALL(SYSCALL2,0,0,0);
    
    }
    */
    finegrain(40);
    /*Which page is missing?
        oldMem ASID register has segment no and page no*/
    missSeg = ((oldState->s_asid & GET_SEG) >> SHIFT_SEG);
    finegrain(5);
    missPage = ((oldState->s_asid & GET_VPN) >> 12);
    debugPager2(13);
    /*Acquire mutex on the swapPool data structure*/
    SYSCALL(SYSCALL4, (int)&swapSem, 0, 0);

    /*    If missing page was from KUseg3, check if the page is still missing
            -check the KUseg3 page table entry's valid bit*/
    if (missPage >= KUSEGSIZE) {
        missPage = KUSEGSIZE - 1;
    }

    /*    Pick a frame to use*/
    newFrame = tableLookUp();
    debugPager2(14);
    /*    If frame is currently occupied
            -turn the valid bit off in the page table of current frame's occupant
            -deal with TLB cache consistency
            -write current frame's contents on nthe backing store*/
    if(swapPool[newFrame].sw_asid != -1){
        InterruptsOnOff(FALSE);
            swapPool[newFrame].sw_pte -> entryLO = ((swapPool[newFrame].sw_pte -> entryLO) & (0 << 9));
            TLBCLR();
        InterruptsOnOff(TRUE);

        MakeTheDiskMyBitch(currentPage, currentASID, 0, 4, swapAddr);

        currentASID = swapPool[newFrame].sw_asid;
        currentPage = swapPool[newFrame].sw_pgNum;
    }

    debugPager2(16);
    /*  Read missing page into selected frame
        Update the swapPool data structure
        Update missing pag's page table entry: frame number and valid bit
        Deal with TLB cache consistency*/

        MakeTheDiskMyBitch(currentPage, currentASID, 0, 3, swapAddr);

        swapPool[newFrame].sw_asid = currentProcessID;
        swapPool[newFrame].sw_segNum = missSeg;
        swapPool[newFrame].sw_pgNum = missPage;
        swapPool[newFrame].sw_pte = &(uProcs[currentProcessID - 1].UProc_pte.pteTable[missPage]);
        debugPager2(20);
        InterruptsOnOff(FALSE);
            swapPool[newFrame].sw_pte -> entryLO = swapAddr | VALID | DIRTY;
            TLBCLR();
        InterruptsOnOff(TRUE);

        if(missSeg == 3){
            swapPool[newFrame].sw_pte = &(kuSeg3.pteTable[missPage]);
            swapPool[newFrame].sw_pte -> entryLO = swapAddr | VALID | DIRTY | GLOBAL;
        }

        

    /*Release mutex and return control to process */        
    SYSCALL(SYSCALL3, (int)&swapSem, 0, 0);
    debugPager2(23);
    debugPager(123); 
    LDST(oldState);
}

void uPgmTrpHandler(){
    debugProg(1);
    /*Grab the ASID*/
    int tempasid; 
    tempasid = ((getENTRYHI() & 0x00000FC0) >> 6);

    /*Kill the process*/
    SYSCALL(SYSCALL2,0,0,0);

}


/*Sys Handler! Switch Statements*/
void uSysHandler(){
    debugSys(1);
    state_t * oldState;
    int casel; 
    int asid; 
    cpu_t times;

    asid = ((getENTRYHI() & 0x00000FC0) >> 6);

    /*Grab the old state Uh oh*/
    oldState = &(uProcs[asid-1].UProc_OldTrap[2]);
    casel = oldState -> s_a0; 

    switch(casel){

        /*Read From Terminal */
        case SYSCALL9:
            readTerminal((char *) oldState->s_a1, asid);
            break;  

        /*Write to Terminal */
        case SYSCALL10:
            writeTerminal((char *) oldState->s_a1, oldState->s_a2 ,asid);
            break;
              
        /*Virtual V (Not Implementing)*/
        case SYSCALL11:
            /*Kill the Process*/
            Endproc(asid);
            /*NO-OP*/
            break;

        /*Virtual P (Not Implementing)*/
        case SYSCALL12:
        /*Kill The Process*/
            Endproc(asid); 
            /*NO-OP*/
            break; 

        /*Delay a Process for N seconds*/
        case SYSCALL13:
            Endproc(asid);
            break;  
        
        /*Disk Put*/
        case SYSCALL14:
            Endproc(asid);
            break;  
        
        /*DISK Get*/
        case SYSCALL15:
            Endproc(asid);
            break;  
        
        /*Write to Printer*/
        case SYSCALL16:
            Endproc(asid);
            break;  
        
        /*Get Time of Day*/
        case SYSCALL17:
            /*Load the time of day and place in v0*/
            STCK(times);
            oldState->s_v0 = times; 
            break;  

        /*Terminate*/
        case SYSCALL18: 
            SYSCALL(SYSCALL2,0,0,0);
            break; 

    }

    LDST(oldState);

}


/*----------------------------------------------*/

void tableLookUp(){
    HIDDEN int nextVal = 0;
    nextVal = (nextVal + 1) % SWAPPOOLSIZE;
    return (nextVal);
}

/*HELPER FUNCTIONS*/

/*Ah so you want to play with a disk? You must make it thy bitch first!
Parameters: 
Give me the block
give me the sector
Give me the disk 
give me is it read or write
give me the PASID:

*/
void MakeTheDiskMyBitch(int block, int sector, int disk, int readWrite, memaddr addr){ 

    int diskStatus;
    devregarea_t* devReg;
	device_t* diskDevice; 

    devReg = (devregarea_t *) RAMBASEADDR;
    diskDevice = &(devReg->devreg[0]);
	
	/*Atomic operation*/
	InterruptsOnOff(FALSE);
    	diskDevice->d_command = (block << 8) | 2;
	    diskStatus = SYSCALL(SYSCALL8, DISKINT, 0, 0);
	InterruptsOnOff(TRUE);
			
	/*If device is done seaking*/
	if(diskStatus == READY){
		
		InterruptsOnOff(FALSE);
		    /*where to read from*/
		    diskDevice->d_data0 = addr;
            /* Command to write*/
            diskDevice->d_command = (disk << 16) | ((sector-1) << 8) | readWrite;
		InterruptsOnOff(TRUE);										   
		
        /*Wait for disk write I/O*/
		diskStatus = SYSCALL(SYSCALL8, DISKINT, 0, 0);

	}else{
        PANIC();
    }

}

void writeTerminal(char* vAddr, int len, int asid)
{
    unsigned int status;
    int i;
    int devNum;
    devregarea_t* devReg;
    device_t* terminal;
    state_t* oldstate;
    
    i = 0;
    devNum = 0 + (asid - 1);
    devReg = (devregarea_t *) RAMBASEADDR;
    terminal = &(devReg -> devreg[devNum]);
    oldstate = (state_t*) &uProcs[asid-1].UProc_OldTrap[2];

    SYSCALL(SYSCALL4, (int)&mutexArr[40 + (asid -1)], 10, 0);

    for(i = 0; i < len; i++)
    {
        InterruptsOnOff(FALSE);
        terminal -> t_transm_command = 2 | (((unsigned int) *vAddr) << 8);
        status = SYSCALL(SYSCALL8, TERMINT, (asid -1), 10);
        InterruptsOnOff(TRUE);

        if((status & 0XFF) != 5)
        {
            PANIC();
        }
        vAddr++;
    }

    oldstate -> s_v0 = i;

    SYSCALL(SYSCALL3, (int)&mutexArr[40 + (asid -1)], 0, 0);
}

void Endproc(int asid){
    TLBCLR(); 

    SYSCALL(SYSCALL2,0,0,0);
}


void readTerminal(char* vAddr, int asid)
{
    /*
    unsigned int status; 
    int i = 0; 
    int devnum = ; 
    devregarea_t* device = (devregarea_t *) RAMBASEADDR;
    device_t* term;
    term = &(device -> devreg[devNum]);
      TODO:
    state_t * OldState =; 

    SYSCALL(SYSCALL4,&mutexArr[], ,0);


    InterruptsOnOff(FALSE); 

    InterruptsOnOff(TRUE);

    */


}








