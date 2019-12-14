/*  PHASE 3
    Written by NICK STONE AND SANTIAGO SALAZAR
    Base comments and some assitance from PROFESSOR MIKEY G 
    Finished on 
*/

/*********************************************************************************************
                            Module Comment Section

    IO module to support IO for virtual memory. This Module consists of Syscalls 9-18 as well as
    responsible for the virtual memory Program trap handler. This support is for accessing the various
    IO Devices. U-proc's will be loaded from tape devices and one of the disk devices will be used
    as the backing store for the VM implementation. U-Proc's will have read/write access to other 
    disk devices, write access to the printers and terminals and optionally read access to the terminals
    as well. The Syscalls implemented are all implemented to be run on processes implemented with Virtual
    Memory On. 

*********************************************************************************************/


#include "../h/const.h"
#include "../h/types.h"

#include "../e/vmIOsupport.e"
#include "../e/initProc.e"

#include "/usr/local/include/umps2/umps/libumps.e"


/*Function declararion... Further details will be given in the actual funciton. */
HIDDEN void Endproc(int asid);
HIDDEN void writeTerminal(char* vAddr, int asid);
HIDDEN void DiskIO(int block, int sector, memaddr addr);
HIDDEN int nextVal = 0;



/*The following functions are testing functions for the function pager*/
void debugPager(int a){}
void debugSys(int a){}
void debugProg(int a){}
void debugPager2(int a){}
void finegrain(int v){}
void debugSyss(int g){}

/*  This module implements the VM-I/O support level TLB exception handler; the Pager. Given 
    that this function is very complex, further details are provided in line comments (here
    it is explained what we do every step*/
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
        debugSyss(3);
        newFrame = tableLookUp(); 
        device = 0x10000000;
    
        debugPager(device);

        debugPager(0);

        /*Calculating RAMTOP*/    
        base = device ->rambase; 
        size = device -> ramsize; 
        thisramtop = base + size; 
        /*thisramtop = (memaddr) ((device->rambase) + (device->ramsize));*/
        debugSyss(4);
        /*Swap Addresss calculation*/   
        swapAddr = (memaddr)(thisramtop - (4*PAGESIZE)) - (newFrame * PAGESIZE);

    /*Who am I?
        The current processID is in the ASID regsiter
        This is needed as the index into the phase 3 global structure*/
    currentProcessID =(int)((getENTRYHI() & GETASID) >> 6);
    oldState = (state_t*) &(uProcs[currentProcessID-1].UProc_OldTrap[TLBTRAP]);

    /*Why are we here? (Examine the oldMem Cause register)*/
    causeReg = (oldState->s_cause);
    causeReg = causeReg << 25;
    causeReg = causeReg >> 27;
    debugSyss(10);
  debugProg(666);
    /*If TLB invalid (load or store) continue; o.w. nuke them*/    
    debugProg(causeReg);
    if(causeReg < 2 || causeReg > 3){
        /*Screwed Up. Nuke the process*/
        SYSCALL(SYSCALL2,0,0,0);
    }

    debugProg(2);

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

    currentASID = (int)((getENTRYHI() & GETASID) >> 6);
    
    /*    If frame is currently occupied
            -turn the valid bit off in the page table of current frame's occupant
            -deal with TLB cache consistency
            -write current frame's contents on nthe backing store*/
    if(swapPool[newFrame].sw_asid != -1){
        /*Atomic Operation*/
        InterruptsOnOff(FALSE);
            swapPool[newFrame].sw_pte -> entryLO = ((swapPool[newFrame].sw_pte -> entryLO) & (0xD << 8));
            TLBCLR();
        InterruptsOnOff(TRUE);
        
        /*Write on disk*/
        DiskIO(currentPage, currentASID-1, swapAddr);
        currentASID = swapPool[newFrame].sw_asid;
        currentPage = swapPool[newFrame].sw_pgNum;
    }

debugPager(3);
    /*Read missing page into selected frame
        Update the swapPool data structure
        Update missing pag's page table entry: frame number and valid bit
        Deal with TLB cache consistency*/

        /*Read from Disk*/
        DiskIO(currentPage, currentASID-1, swapAddr);
        debugSyss(12);
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

    debugSyss(30);
    /*Turns VM back off*/    
    /*setSTATUS(ALLOFF | IMON | IEON | TEON | VMOFF);*/

    LDST(oldState);
}

/*  The nucleus either treats a PgmTrap exception as a SYS2 or if the offending process issues a
    SYS5 for PgmTrap exceptions it passes up the handling of the exception. Asumming everything
    was correct in the state, execution continues with the PgmTrap exception handler*/
void uPgmTrpHandler(){
    int tempasid;   /*Grab the ASID*/
    tempasid = ((getENTRYHI() & 0x00000FC0) >> 6);

    /*Kill the process*/
    SYSCALL(SYSCALL18,0,0,0);
}

/*  This module implements the VM-I/O support level SYS/Bp and PgmTrap exception handlers. For testing
    purposes only two SYSCALLS have been created (Write Terminal Syscall and Get Time of Day Syscall).
    However, the main logical structure is provided (no code is executed in here)*/
void uSysHandler(){
    state_t * oldState;
    int casel; 
    int asid; 
    cpu_t times;

    /*Get asid*/
    asid = ((getENTRYHI() & 0x00000FC0) >> 6);

    /*Get the old state*/
    oldState = &(uProcs[asid-1].UProc_OldTrap[SYSTRAP]);
    casel = oldState -> s_a0; 

    /*Switch case scenario (what SYS are we executing)*/
    switch(casel){

        /*Read From Terminal (Not Implementing) */
        case SYSCALL9:
            SYSCALL(SYSCALL18,0,0,0);
            break;  

        /*Write to Terminal */
        case SYSCALL10:
            writeTerminal((char *) oldState->s_a1,asid);
            break;
              
        /*Virtual V (Not Implementing)*/
        case SYSCALL11:
            /*Kill the Process*/
             SYSCALL(SYSCALL18,0,0,0);
            break;

        /*Virtual P (Not Implementing)*/
        case SYSCALL12:
            /*Kill The Process*/
             SYSCALL(SYSCALL18,0,0,0);
            break; 

        /*Delay a Process for N seconds (Not Implementing)*/
        case SYSCALL13:
             SYSCALL(SYSCALL18,0,0,0);
            break;  
        
        /*Disk Put (Not Implementing)*/
        case SYSCALL14:
            SYSCALL(SYSCALL18,0,0,0);
            break;  
        
        /*DISK Get (Not Implementing)*/
        case SYSCALL15:
           SYSCALL(SYSCALL18,0,0,0);
            break;  
        
        /*Write to Printer (Not Implementing)*/
        case SYSCALL16:
             SYSCALL(SYSCALL18,0,0,0);
            break;  
        
        /*Get Time of Day*/
        case SYSCALL17:
            /*Load the time of day and place in v0*/
            STCK(times);
            oldState->s_v0 = times; 
            break;  

        /*Terminate process*/
        case SYSCALL18: 
            SYSCALL(SYSCALL4,&swapSem,0,0);
            InterruptsOnOff(FALSE);
            int i; 
            int tasid = ((getENTRYHI() & 0x00000FC0) >> 6);
            for(i = 0; i < SWAPPOOLSIZE;i++){
                if(swapPool[i].sw_asid == tasid ){
                    swapPool[i].sw_asid = -1; 
                    swapPool[i].sw_segNum = 0;
                    swapPool[i].sw_pgNum = 0;
                    swapPool[i].sw_pte = NULL;
                }

            }
            InterruptsOnOff(TRUE);
            SYSCALL(SYSCALL3,&swapSem,0,0);
            SYSCALL(SYSCALL3,&masterSem,0,0);
            /*writeTerminal("Recursive Fibanaci Test starts\n13\nRecursion Concluded\nRecursion Concluded Successfully\n",asid);*/
            TLBCLR(); 
            SYSCALL(SYSCALL2,0,0,0);
            break; 
    }

    LDST(oldState);

}

/*--------------------------------HELPER FUNCTIONS----------------------------*/

/*  Looks up the next available frame in the memory pool. The hashing of this function
    is very simple. It just looks looks for the inmmediate next entry unless it is at
    very end of the table, then it will go back to the top.*/
void tableLookUp(){
    nextVal = (nextVal + 1) % SWAPPOOLSIZE;
    return (nextVal);
}

/*  Writes or reads a disk device from a different tape device. If done succesfully, it should
    produce a status(READY) and end the process (Every reading and writing are performed in an atomic
    operation). If an error occurs along the way, means that ksegOS is an error and result in the
    U-proc being terminated (SYS18)*/
void DiskIO(int block, int sector, memaddr addr){ 
    debugPager(3345);
    int diskStatus;
    devregarea_t* devReg;
	device_t* diskDevice; 
    debugPager(34);
    devReg = (devregarea_t *) RAMBASEADDR;
    diskDevice = &(devReg->devreg[0]);
    debugPager(10); 

    int headofdisk = 0;  
    int sectornumber = sector << 3; 
    sector = sector >> 1;
    /*Seek the Cylinder */
    InterruptsOnOff(FALSE);
    	debugPager(12);
        diskDevice->d_command = (sector << 8) | 2;
        debugPager(10);
        diskStatus = SYSCALL(SYSCALL8, 3, 0, 0);
        debugPager(2);
    InterruptsOnOff(TRUE);

    debugPager(20);

    


	/*Atomic operation*/
	InterruptsOnOff(FALSE);
    	diskDevice->d_command = (headofdisk) | (sectornumber << 8) |  3;
        diskDevice->d_data0 = (ROMPAGESTART + (30 * PAGESIZE)) + ((1 - 1) * PAGESIZE);
        diskStatus = SYSCALL(SYSCALL8, DISKINT, 0, 0);
    InterruptsOnOff(TRUE);
			
    writeTerminal((char *) "HEY", 1);
    

    debugPager(diskStatus);    
	/*If device is done seaking*/
	if(diskStatus == READY){

    debugPager(3330);  
		InterruptsOnOff(FALSE);
        
    debugPager(330);  
		    /*where to read from*/
		    diskDevice->d_data0 = addr;
           
    debugPager(340);  
            /* Command to write*/
            diskDevice->d_command = (headofdisk << 16) | ((sectornumber) << 8) | 4;
	
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


/*  This syscall causes the requesting U-proc to be suspended until a line of output (string of
     characters) has been transmitted to the terminal device associated with the U-proc.*/
void writeTerminal(char* msg, int asid)
{
    char * s = msg;
	unsigned int * base = (unsigned int *) (0x10000250);
	unsigned int status;
	
	SYSCALL(SYSCALL4, (int)&mutexArr[0], 0, 0);				/* P(term_mut) */
	while (*s != EOS) {
		*(base + 3) = 2 | (((unsigned int) *s) << 8);
		status = SYSCALL(SYSCALL8, TERMINT, asid-1, 0);	
		if ((status & 0xFF) != 5)
			PANIC();
		s++;	
	}
	SYSCALL(SYSCALL3, (int)&mutexArr[0], 0, 0);				/* V(term_mut) */
}

/*  This services causes the executing U-proc to cease to exist. When all U-proc’s have terminated,
    Kaya should “shut down.” Thus, system processes created in the VM-I/O support level will be
    terminated after all the U-proc’s have been killed*/
void Endproc(int asid){
    TLBCLR(); 
    SYSCALL(SYSCALL2,0,0,0);
}










