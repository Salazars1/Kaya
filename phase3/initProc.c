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

#include "../e/initial.e"
#include "../e/exceptions.e"

#include "../e/initProc.e"
#include "../e/vmIOsupport.e"

#include "/usr/local/include/umps2/umps/libumps.e"


/*Lets set some global values*/
pteOS_t KSegOS; /*OS page table*/
pte_t kuSeg3;   /*Seg3 page table*/
swap_t swapPool[SWAPPOOLSIZE];

/*Semaphore globals*/
int swapSem;
int mutexArr[SEMNUM];
int masterSem;

/*Array of processes (8)*/
uProc_t uProcs[MAXUPROC];

/*Lets Extern some functions*/
extern void pager();
extern void uPgmTrpHandler();
extern void uSysHandler();

/*Declare this function */
HIDDEN void uProcInit();

/*Test Functions for test()*/
void debug(int a){}

/*Called in the Initial.c File from phase2 (Our Main)*/
/*TODO:*/
void test()
{
    /*Looping variables*/
    int i;
    int j;
   
    state_t procState;      /*Process state*/
    segTable_t* segTable;   /*Seg table*/

    /*KSegOS page table
        64 entries
            entryHI= 0x20000+i
            entryLO= 0x20000+i w/Dirty, Global, Valid
    */
   KSegOS.header = (0x2A<<24)|KSEGSIZE;
   for(i=0;i<KSEGSIZE;i++){
       /*Set the Page tables */
       KSegOS.pteTable[i].entryHI = ((0x20000 + i) << 12);
       KSegOS.pteTable[i].entryLO = ((0x20000 + i) << 12)| DIRTY | GLOBAL | VALID;
    }

   /*kuSeg3 page table
        32 entries
            entryHI= 0xC0000+i
            entryLO= Dirty, Global
    */
   kuSeg3.header = (0x2A<<24)|KUSEGSIZE;
   for(i=0;i<KUSEGSIZE;i++){
       /*Set the Page tables */
       KSegOS.pteTable[i].entryHI = ((0xC0000 + i)<< 12)|(ZERO <<6);
       KSegOS.pteTable[i].entryLO = ALLOFF | DIRTY | GLOBAL;
    }

   /*Swap Pool Data Structure
        Set each entry to "unoccopied"
            -ASID (-1)
            -Seg Number
            -Page Number
            -(optional) pointer to a pte_t
    */
   for(i = 0; i < SWAPPOOLSIZE; i++){
       /*Set each of the entries in the frame pool */
       swapPool[i].sw_asid = -1;
       swapPool[i].sw_segNum = 0;
       swapPool[i].sw_pgNum = 0;
       swapPool[i].sw_pte = NULL;
    }

    /*swap pool sema4
        init to 1
    */
    swapSem = 1;        /*Mutual Exclusion Semaphore thus resulting in the Value of 1*/

    /*an array of sempahores: one for each interrupting device
        -init to 1
    */
    for(i=0; i< SEMNUM; i++){
        mutexArr[i] = 1;    /*Array of mutual exclusion semaphores */
    }
   
    /*MasterSema4
        -init to 0
    */
    masterSem = 0;      /*This semaphore is a mutual exclusion semaphore and is thus set to 0 */

   /*Process initialization loop (for (i=1; i<MAXUPROC+1;i++)){
       --initialize stuff
       --SYS 1
    }*/
    for(i =1; i< MAXUPROC+1;i++){
        /* i becomes the ASID (processID)*/
        uProcs[i-1].UProc_pte.header = (0x2A<<24)|KUSEGSIZE;

        /*KUseg2 page table
            -32 entries:
                entryHI=0x80000+i w/asid
                entryLO = Dirty
        */
        for(j = 0; j < KUSEGSIZE; j++)
        {
            /*set the page table associated with each process*/
            uProcs[i-1].UProc_pte.pteTable[j].entryHI =((0x80000 + j) << 12) | (i << 6);
            uProcs[i-1].UProc_pte.pteTable[j].entryLO = ALLOFF | DIRTY;
        }

        /*fix the last entry's entryHi = 0xBFFFF w/asid*/
        uProcs[i-1].UProc_pte.pteTable[KUSEGSIZE-1].entryHI = (0xBFFFF << 12) | (i << 6); 

        /*Set up the appropiate three entries in the global segment table
            set KSegOS pointer
            set KUseg2 pointer
            set KUseg3 pointer
        */

        /*The width of the seg table instead of the Seg table start!*/
        segTable = (segTable_t *) (0x20000500 + (i * 0x0000000C));

        /*Set the Seg tables*/
        segTable->ksegOS= &KSegOS;
        segTable->kuseg2= &(uProcs[i-1].UProc_pte);
        /*segTable->kuseg3= &kuSeg3;*/

        /*Set up an initial state for a user process
            -asid =i
            -stack page = tp be filled in later
            -PC = uProcInit
            -status: all interrupts enabled, local timer enabled, VM off, kernel mode on
        */
        procState.s_asid= (i<<6);              /*Set the asid*/
        procState.s_sp = ALLOCATEHERE + ((i-1) * BASESTACKALLOC);            /*Take the address of the the base that we can allocate then allocate a unique address with 2 pages of memory */
        procState.s_pc = (memaddr) uProcInit;
        procState.s_t9 = (memaddr) uProcInit;
        procState.s_status = ALLOFF | IEON | IMON | TEBITON;
        uProcs[i-1].UProc_semAdd = 0; 

        /*Create Process*/
        SYSCALL(SYSCALL1, (int)&procState, 0, 0);
    }

    /*for (i=0; i<MAXUPROC; i++){
        P(MasterSema4);}
    */
   for (i = 0; i < MAXUPROC; i++)
   {
       /*Gain access of the master semaphore for the 8 processes */
       SYSCALL(SYSCALL4, (int) &masterSem, 0, 0);
   }
   
    /*TERMINATE*/
    SYSCALL(SYSCALL2, 0, 0, 0);

}

/*TODO:*/
void uProcInit()
{
    /*Set a series of local variables*/
    unsigned int asid;
    int deviceNo;
    
    /*Set states for the TLB PROGRAM TRAP AND SYS HANDLER*/
    state_t* newStateTLB;
    state_t* newStatePRG;
    state_t* newStateSYS;
    state_t  stateProc;
    
    /*Set the memory addresses of the TLB PROGRAM TRAP AND SYS HANDLER*/
    memaddr TLBTOP;
    memaddr PROGTOP;
    memaddr SYSTOP;

    /*Figure out who you are? ASID?*/
    asid = getENTRYHI();
    asid = (asid & 0x00000FC0) >> 6;
     
    /*Calculating program tops*/
    PROGTOP = ALLOCATEHERE + ((asid-1) * BASESTACKALLOC);
    SYSTOP = ALLOCATEHERE + ((asid-1) * BASESTACKALLOC);
    TLBTOP = PROGTOP - PAGESIZE;

    state_t * oldStateTLB; 
    state_t* oldStatePRG; 
    state_t * oldStateSYS; 
    
    /*Set up three new areas for Pass Up or Die
        -stack page: to be filled in later
        -PC = address of your Phase 3 handler
        -ASID = your asid value
        -status: all interrupts enabled, local timer enabled, VM ON, Kernel Mode ON
    */

    /*Create a Pager Handler*/
    newStateTLB = &(uProcs[asid-1].UProc_NewTrap[TLBTRAP]);
    oldStateTLB = &(uProcs[asid-1].UProc_OldTrap[TLBTRAP]);
    newStateTLB->s_sp = TLBTOP;
    newStateTLB->s_pc = (memaddr) pager;
    newStateTLB->s_t9 = (memaddr) pager;
    newStateTLB->s_asid = (asid << 6);
    newStateTLB->s_status = ALLOFF | IMON | IEON | TEON | VMOFF | UMOFF;

    /*Create a Program Trap handler state*/
    newStatePRG = &(uProcs[asid-1].UProc_NewTrap[PROGTRAP]);
    oldStatePRG = &(uProcs[asid-1].UProc_OldTrap[PROGTRAP]);
    newStatePRG->s_sp = PROGTOP;
    newStatePRG->s_pc = (memaddr) uPgmTrpHandler;
    newStatePRG->s_t9 = (memaddr) uPgmTrpHandler;
    newStatePRG->s_asid = (asid << 6);
    newStatePRG->s_status = ALLOFF | IMON | IEON | TEON | VMON2 ;

    /*Create a Syshandler State*/
    newStateSYS = &(uProcs[asid-1].UProc_NewTrap[SYSTRAP]);
    oldStateSYS = &(uProcs[asid-1].UProc_OldTrap[SYSTRAP]);
    newStateSYS->s_sp = SYSTOP;
    newStateSYS->s_pc = (memaddr) uSysHandler;
    newStateSYS->s_t9 = (memaddr) uSysHandler;
    newStateSYS->s_asid = (asid << 6);
    newStateSYS->s_status = ALLOFF | IMON | IEON | TEON | VMON2;

    /*Call SYS 5, three times on these new regions*/
    SYSCALL(SYSCALL5,TLBTRAP,(int)oldStateTLB,(int) newStateTLB);
    SYSCALL(SYSCALL5,PROGTRAP,(int) oldStatePRG,(int) newStatePRG);
    SYSCALL(SYSCALL5,SYSTRAP,(int) oldStateSYS,(int) newStateSYS);

    deviceNo = ((TAPEINT - 3) * DEVPERINT) + (asid - 1);

   /*Read the content of the tape devices(asid-1) on the the backing store device (disk0)
       keep reading until the tape block marker (data1) is no longer ENDOFBLOCK
       read block from tape and then write it out to disk0*/

    /*Mutual exclusion on the device*/
    SYSCALL(SYSCALL4, (int) &mutexArr[deviceNo], 0, 0);

    /*Device declaration*/
    device_t* tape;
    device_t* disk;
    devregarea_t * Activedev;

    int buffer;
    int pageNumber;
    unsigned int tapeStatus;
    unsigned int diskStatus;

    Activedev= RAMBASEADDR; 
    pageNumber=0;

    disk = &(Activedev -> devreg[0]);           /*Backing Store is at Number 0 !*/
    tape = &(Activedev ->devreg[deviceNo]);     /*The tape is a dynamic number */

    /*Setting variables to be used in reading into tape and placing on backing store (Disk 0) */
    int bool = 0;
    tapeStatus= READY;
    int finished;
    finished=FALSE;

    /* loop until whole file has been read */
	while((tapeStatus==READY) && !finished) {
        /*Atomic operation (READ FROM TAPE)*/
        InterruptsOnOff(FALSE);
		    tape -> d_data0 = (ROMPAGESTART + (30 * PAGESIZE)) + ((asid - 1) * PAGESIZE);
		    tape -> d_command = DISKREADBLK;
            tapeStatus = SYSCALL(SYSCALL8, TAPEINT, (asid-1), 0);
        InterruptsOnOff(TRUE);
        
        /*MUTUAL EXCLUSION ON DISK*/
        SYSCALL(SYSCALL4, (int) &mutexArr[0], 0, 0);

        /*Atomic operation (IS THE DISK READY?)*/
        InterruptsOnOff(FALSE);
            disk ->d_command = (pageNumber << 8 | 2);
            diskStatus = SYSCALL(SYSCALL8, DISKINT, 0, 0);
        InterruptsOnOff(TRUE);
        
        if(diskStatus == READY)
        {
            /*Atomic operation (WRITE IT ONTO THE DISK)*/
            InterruptsOnOff(FALSE);
                disk->d_data0 = (ROMPAGESTART + (30 * PAGESIZE)) + ((asid - 1) * PAGESIZE);
                disk->d_command = ((asid - 1) << 8) | 4;
                diskStatus = SYSCALL(SYSCALL8,DISKINT,0,0);
            InterruptsOnOff(TRUE);
        }
        /*RELEASE MUTUAL EXCLUSION ON DISK*/
        SYSCALL(SYSCALL3, (int) &mutexArr[0], 0, 0);

        if(tape->d_data1 != 2){
            finished = TRUE;
        }
        pageNumber++;
	}

    SYSCALL(SYSCALL3, (int) &mutexArr[deviceNo], 0, 0);

    /*Set up a new state for the user process
        -asid = your asid
        -stack page = last page of KUseg2(0C00.0000)
        -status: all interrupts enabled, local timer enabled, VM ON, User mode ON
        -PC = well known address from the start of KUseg2
    */

   /*Establish a new state that points the "Pager" Such that we can handle virtual memory*/
    stateProc.s_asid = asid << 6;
    stateProc.s_sp = (memaddr) SEG3;
    stateProc.s_status = ALLOFF | IEON | IMON | TEBITON | UMOFF | VMON2;
    stateProc.s_pc = (memaddr) WELLKNOWNSTARTPROCESS; 
    stateProc.s_t9 = (memaddr) WELLKNOWNSTARTPROCESS;
    
   /*Loadstate to the new state that we have just created*/
   LDST(&stateProc);
}

/*Function to toggle interrupts on and off*/
 void InterruptsOnOff(int IOturned)
{
    /*Get the current status of the process*/
    int status = getSTATUS();
    /*If false*/
    if(!IOturned)
    {
        /*Set the status to interrupts off*/
        status = (status & 0xFFFFFFFE);
    }
    /*If true*/
    else
    {
        /*Set the status to turn interrupts on */
        status = (status | 0x1);
    }
    /*Update the new status to include or exlude interrupts*/
    setSTATUS(status);
}
