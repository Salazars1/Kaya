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
HIDDEN void writePrinter(char* vAddr, int len, int asid);
HIDDEN void MakeTheDiskMyBitch(int block, int sector, int disk, int readWrite, memaddr addr);

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
    int currentPage;
    int currentASID;


        device = (devregarea_t*) RAMBASEADDR;
        RAMTOP = (device->rambase) + (device->ramsize);
        swapAddr = (RAMTOP - ((16 + 3)*PAGESIZE)) + (newFrame * PAGESIZE);;


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

    /*    Pick a frame to use*/
    newFrame = tableLookUp();

    /*    If frame is currently occupied
            -turn the valid bit off in the page table of current frame's occupant
            -deal with TLB cache consistency
            -write current frame's contents on nthe backing store*/
    if(swapPool[newFrame].sw_asid != -1){
        Interrupts(FALSE);
            swapPool[newFrame].sw_pte -> entryLO = ((swapPool[newFrame].sw_pte -> entryLO) & (0 << 9));
            TLBCLR();
        Interrupts(TRUE);

        MakeTheDiskMyBitch(currentPage, currentASID, 0, 4, swapAddr);

        currentASID = swapPool[newFrame].sw_asid;
        currentPage = swapPool[newFrame].sw_pgNum;
    }


    /*  Read missing page into selected frame
        Update the swapPool data structure
        Update missing pag's page table entry: frame number and valid bit
        Deal with TLB cache consistency*/

        MakeTheDiskMyBitch(currentPage, currentASID, 0, 3, swapAddr);

        swapPool[newFrame].sw_asid = currentProcessID;
        swapPool[newFrame].sw_segNum = missSeg;
        swapPool[newFrame].sw_pgNum = missPage;
        swapPool[newFrame].sw_pte = &(uProcs[currentProcessID - 1].UProc_pte.pteTable[missPage]);
        
        Interrupts(FALSE);
            swapPool[newFrame].sw_pte -> entryLO = swapAddr | VALID | DIRTY;
            TLBCLR();
        Interrupts(TRUE);

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
    int tempasid; 
    tempasid = ((getENTRYHI() & 0x00000FC0) >> 6);

    /*Kill the process*/
    SYSCALL(SYSCALL2,0,0,0);

}


/*Sys Handler! Switch Statements*/
void uSysHandler(){
    state_t * oldState;
    int casel; 
    int asid; 
    cpu_t times;

    asid = getENTRYHI();
    asid = (asid & 0x00000FC0) >> 6;

    /*Grab the old state Uh oh*/
    oldState = &(uProcs[asid-1].uprocOldTrap[2]);
    casel = oldState -> s_a0; 

    switch(casel){

        /*Read From Terminal */
        case 9:
            readTerminal((char *) oldState->s_a1, asid);
            break;  

        /*Write to Terminal */
        case 10:
            writeTerminal((char *) oldState->s_a1, oldState->s_a2 ,asid);
            break;
              
        /*Virtual V (Not Implementing)*/
        case 11:
            /*Kill the Process*/
            Endproc(asid);
            /*NO-OP*/
            break;

        /*Virtual P (Not Implementing)*/
        case 12:
        /*Kill The Process*/
            Endproc(asid); 
            /*NO-OP*/
            break; 

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
            writeprinter();
            break;  
        
        /*Get Time of Day*/
        case 17:
            /*Load the time of day and place in v0*/
            STCK(times);
            oldState->s_v0 = times; 
            break;  

        /*Terminate*/
        case 18: 
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
	Interrupts(FALSE);
    	diskDevice->d_command = (block << 8) | 2;
	    diskStatus = SYSCALL(SYSCALL8, DISKINT, 0, 0);
	Interrupts(TRUE);
			
	/*If device is done seaking*/
	if(diskStatus == READY){
		
		Interrupts(FALSE);
		    /*where to read from*/
		    diskDevice->d_data0 = addr;
            /* Command to write*/
            diskDevice->d_command = (disk << 16) | ((sector-1) << 8) | readWrite;
		Interrupts(TRUE);										   
		
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
    devNum = 32 + (asid - 1);
    devReg = (devregarea_t *) RAMBASEADDR;
    terminal = &(devReg -> devreg[devNum]);
    oldstate = (state_t*) &uProcs[asid-1].UProc_OldTrap[2];

    SYSCALL(SYSCALL4, (int)&mutexArr[40 + (asid -1)], 0, 0);

    while(i < len)
    {
        Interrupts(FALSE);
        terminal -> t_transm_command = 2 | (((unsigned int) *vAddr) << 8);
        status = SYSCALL(SYSCALL8, TERMINT, (asid -1), 0);
        Interrupts(TRUE);

        if((status & 0XFF) != 5)
        {
            PANIC();
        }
        vAddr++;
        i++;
    }

    oldstate -> s_v0 = i;

    SYSCALL(SYSCALL3, (int)&mutexArr[40 + (asid -1)], 0, 0);
}

void Endproc(int asid){

    Interrupts(FALSE);

    Interrupts(TRUE);

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


    Interrupts(FALSE); 

    Interrupts(TRUE);

    */


}





