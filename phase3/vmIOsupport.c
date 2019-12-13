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


/*Function declararion...*/
HIDDEN void Endproc(int asid);
HIDDEN void writeTerminal(char* vAddr, int len, int asid);
HIDDEN void DiskIO(int block, int sector, int disk, int readWrite, memaddr addr);


/*THe following functions are testing functions for the function pager*/
void debugPager(int a){}
void debugSys(int a){}
void debugProg(int a){}
void debugPager2(int a){}
void finegrain(int v){}

/*TODO:*/
void pager()
{

    state_t* oldState;
    devregarea_t* device;

    /*Memory Addresses to be computed later*/
    memaddr thisramtop;
    memaddr swapAddr;

    /*Variables to be used later in the program.*/
    int currentASID;
    int currentProcessID;
    int causeReg;
    int missSeg;
    int missPage;
    int newFrame;
    int currentPage;
    
    /*RAMTOP componets*/
    unsigned int base; 
    unsigned int size; 

    /*Turns VM on*/
    setSTATUS( ALLOFF | IEON | IMON | TEBITON | UMOFF | VMON2);
    
        newFrame = tableLookUp(); 
        device = 0x10000050;
        
        debugPager(device);

        debugPager(0);

        /*Calculating RAMTOP*/    
        base = device ->rambase; 
        size = device -> ramsize; 
        thisramtop = base + size; 
        /*thisramtop = (memaddr) ((device->rambase) + (device->ramsize));*/

        /*Swap Addresss calculation*/   
        swapAddr = (memaddr)(thisramtop - ((16 + 3)*PAGESIZE)) + (newFrame * PAGESIZE);

    /*Turns VM back off*/    
    /*setSTATUS(ALLOFF | IMON | IEON | TEON | VMOFF);*/
    

    /*Who am I?
        The current processID is in the ASID regsiter
        This is needed as the index into the phase 3 global structure*/
    currentProcessID = (int)((getENTRYHI() & GETASID) >> 6);
    oldState = (state_t*) &(uProcs[currentProcessID-1].UProc_OldTrap[TLBTRAP]);
    
    /*Why are we here? (Examine the oldMem Cause register)*/
    causeReg = (oldState->s_cause);
  
    /*If TLB invalid (load or store) continue; o.w. nuke them*/    
    if(1==0){
    /*if(currentProcessID < 2 || currentProcessID > 3){*/
        /*Screwed Up. Nuke the process*/
        SYSCALL(SYSCALL2,0,0,0);
    }

    /*if((checkthisid != 2) || (checkthisid!= 3)){
        debugPager2(4);
        SYSCALL(SYSCALL2,0,0,0);    
    }
    */
    
    /*Which page is missing?
        -oldMem ASID register has segment no and page no*/
    missSeg = ((oldState->s_asid & GET_SEG) >> SHIFT_SEG);
    missPage = ((oldState->s_asid & GET_VPN) >> 12);

    /*GET MUTUAL EXCLUSION on Swap Semaphore*/
    SYSCALL(SYSCALL4, (int)&swapSem, 0, 0);

    /*If missing page was from KUseg3, check if the page is still missing
        -check the KUseg3 page table entry's valid bit*/
    if (missPage >= KUSEGSIZE) {
        missPage = KUSEGSIZE - 1;
    }

    /*Pick a frame to use*/
    newFrame = tableLookUp();
    
    /*    If frame is currently occupied
            -turn the valid bit off in the page table of current frame's occupant
            -deal with TLB cache consistency
            -write current frame's contents on nthe backing store*/
    if(swapPool[newFrame].sw_asid != -1){
        /*Atomic Operation*/
        InterruptsOnOff(FALSE);
            swapPool[newFrame].sw_pte -> entryLO = ((swapPool[newFrame].sw_pte -> entryLO) & (0 << 9));
            TLBCLR();
        InterruptsOnOff(TRUE);
        
        /*Write on disk*/
        DiskIO(currentPage, currentASID, 0, 4, swapAddr);
        currentASID = swapPool[newFrame].sw_asid;
        currentPage = swapPool[newFrame].sw_pgNum;
    }

debugPager(3);
    /*Read missing page into selected frame
        Update the swapPool data structure
        Update missing pag's page table entry: frame number and valid bit
        Deal with TLB cache consistency*/

        /*Read from Disk*/
        DiskIO(currentPage, currentASID, 0, 3, swapAddr);
debugPager(4);
        swapPool[newFrame].sw_asid = currentProcessID;
        swapPool[newFrame].sw_segNum = missSeg;
        swapPool[newFrame].sw_pgNum = missPage;
        swapPool[newFrame].sw_pte = &(uProcs[currentProcessID - 1].UProc_pte.pteTable[missPage]);
debugPager(5);        
        /*Atomic Operation*/
        InterruptsOnOff(FALSE);
            swapPool[newFrame].sw_pte -> entryLO = swapAddr | VALID | DIRTY;
            TLBCLR();
        InterruptsOnOff(TRUE);
debugPager(6);
        if(missSeg == 3){
            swapPool[newFrame].sw_pte = &(kuSeg3.pteTable[missPage]);
            swapPool[newFrame].sw_pte -> entryLO = swapAddr | VALID | DIRTY | GLOBAL;
        }

        

    /*Release mutex and return control to process */        
    SYSCALL(SYSCALL3, (int)&swapSem, 0, 0);
    debugPager(4);
    LDST(oldState);
}

/*TODO:*/
void uPgmTrpHandler(){
    int tempasid;   /*Grab the ASID*/
    tempasid = ((getENTRYHI() & 0x00000FC0) >> 6);

    /*Kill the process*/
    SYSCALL(SYSCALL2,0,0,0);
}

/*TODO:*/
void uSysHandler(){
    state_t * oldState;
    int casel; 
    int asid; 
    cpu_t times;

    /*Get asid*/
    asid = ((getENTRYHI() & 0x00000FC0) >> 6);

    /*Get the old state*/
    oldState = &(uProcs[asid-1].UProc_OldTrap[2]);
    casel = oldState -> s_a0; 

    /*Switch case scenario (what SYS are we executing)*/
    switch(casel){

        /*Read From Terminal (Not Implementing) */
        case SYSCALL9:
            break;  

        /*Write to Terminal */
        case SYSCALL10:
            writeTerminal((char *) oldState->s_a1, oldState->s_a2 ,asid);
            break;
              
        /*Virtual V (Not Implementing)*/
        case SYSCALL11:
            /*Kill the Process*/
            Endproc(asid);
            break;

        /*Virtual P (Not Implementing)*/
        case SYSCALL12:
            /*Kill The Process*/
            Endproc(asid); 
            break; 

        /*Delay a Process for N seconds (Not Implementing)*/
        case SYSCALL13:
            Endproc(asid);
            break;  
        
        /*Disk Put (Not Implementing)*/
        case SYSCALL14:
            Endproc(asid);
            break;  
        
        /*DISK Get (Not Implementing)*/
        case SYSCALL15:
            Endproc(asid);
            break;  
        
        /*Write to Printer (Not Implementing)*/
        case SYSCALL16:
            Endproc(asid);
            break;  
        
        /*Get Time of Day*/
        case SYSCALL17:
            /*Load the time of day and place in v0*/
            STCK(times);
            oldState->s_v0 = times; 
            break;  

        /*Terminate process*/
        case SYSCALL18: 
            SYSCALL(SYSCALL2,0,0,0);
            break; 
    }

    LDST(oldState);

}


/*----------------------------------------------*/

/*TODO:*/
void tableLookUp(){
    HIDDEN int nextVal = 0;
    nextVal = (nextVal + 1) % SWAPPOOLSIZE;
    return (nextVal);
}

/*--------------------------------HELPER FUNCTIONS--------------------*/

/*Ah so you want to play with a disk? You must make it thy bitch first!
Parameters: 
Give me the block
give me the sector
Give me the disk 
give me is it read or write
give me the PASID:

TODO:*/
void DiskIO(int block, int sector, int disk, int readWrite, memaddr addr){ 
    debugPager(3345);
    int diskStatus;
    devregarea_t* devReg;
	device_t* diskDevice; 
    debugPager(34);
    devReg = (devregarea_t *) RAMBASEADDR;
    diskDevice = &(devReg->devreg[0]);
	debugPager(15);
	/*Atomic operation*/
	InterruptsOnOff(FALSE);
    	diskDevice->d_command = (block << 8) | 2;
        debugPager(18);
        diskStatus = SYSCALL(SYSCALL8, DISKINT, 0, 0);
        debugPager(19);
    InterruptsOnOff(TRUE);
			

    debugPager(320);    
	/*If device is done seaking*/
	if(diskStatus == READY){

    debugPager(3330);  
		InterruptsOnOff(FALSE);
        
    debugPager(330);  
		    /*where to read from*/
		    diskDevice->d_data0 = addr;
           
    debugPager(340);  
            /* Command to write*/
            diskDevice->d_command = (disk << 16) | ((sector-1) << 8) | readWrite;
	
    debugPager(306);  
    	InterruptsOnOff(TRUE);										   
		
    debugPager(3077);  
        /*Wait for disk write I/O*/
		diskStatus = SYSCALL(SYSCALL8, DISKINT, 0, 0);

    debugPager(37);  
	}else{
        
    debugPager(35);  
        /*PANIC*/
        PANIC();
    }


}

/*TODO:*/
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

    /*GETS MUTUAL EXCUSION ON DEVICE NUMBER*/
    SYSCALL(SYSCALL4, (int)&mutexArr[40 + (asid -1)], 10, 0);

    /*Prints to the terminal */
    for(i = 0; i < len; i++)
    {
        /*Atomic Operation*/
        InterruptsOnOff(FALSE);
            terminal -> t_transm_command = 2 | (((unsigned int) *vAddr) << 8);
            status = SYSCALL(SYSCALL8, TERMINT, (asid -1), 10);
        InterruptsOnOff(TRUE);

        if((status & 0XFF) != 5)
        {
            PANIC();
        }
        /*Updates pointer (next letter)*/
        vAddr++;
    }

    oldstate -> s_v0 = i;
    
    /*RELEASE MUTUTAL EXCLUSION ON DEVICE NUMBER*/
    SYSCALL(SYSCALL3, (int)&mutexArr[40 + (asid -1)], 0, 0);
}

/*TODO:*/
void Endproc(int asid){
    TLBCLR(); 
    SYSCALL(SYSCALL2,0,0,0);
}










