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
/*Make this function an extern */
HIDDEN void uProcInit();

/*Test Functions for test()*/
void debug(int a){}
void testingbi(int b){}
void testa(int b){}
void te(int c){}
void re(int d){}

/*Test Functions for Uprocint()*/
void a(int f){}
void ab(int f){}
void abc(int f){}



/*Functions for testing the while loop in Uprocint*/
void whi(int b){}
void wh(int cc){}
void h(char a){}



/*Called in the Initial.c File from phase2 (Our Main)*/
void test()
{
    /*Looping variables*/
    int i;
    int j;
    /*Process state*/
    state_t procState;
    /*Seg table*/
    segTable_t* segTable;

    /*KSegOS page table
        64 entries
            entryHI= 0x20000+i
            entryLO= 0x20000+i w/Dirty, Global, Valid
    */
   KSegOS.header = (0x2A<<24)|KSEGSIZE;
   for(i=0;i<KSEGSIZE;i++){
       /*Set the Page tables */
       KSegOS.pteTable[i].entryHI = ((0x20000 + i) << 12);
       KSegOS.pteTable[i].entryLO = ((0x20000 + i) << 12);
        /*| DIRTY | GLOBAL | VALID;*/
    }

   /*kuSeg3 page table
        32 entries
            entryHI= 0xC0000+i
            entryLO= Dirty, Global
    */
   kuSeg3.header = (0x2A<<24)|KUSEGSIZE;
   for(i=0;i<KUSEGSIZE;i++){
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
   /*Mutual Exclusion Semaphore thus resulting in the Value of 1*/
    swapSem = 1;

    /*an array of sempahores: one for each interrupting device
        -init to 1
    */
    for(i=0; i< SEMNUM; i++){
        /*Array of mutual exclusion semaphores */
        mutexArr[i] = 1;
    }
   
    /*MasterSema4
        -init to 0
    */
   /*This semaphore is a mutual exclusion semaphore and is thus set to 0 */
    masterSem = 0;

   /*Process initialization loop (for (i=1; i<MAXUPROC+1;i++)){
       --initialize stuff
       --SYS 1
    }   
    */
    

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
            uProcs[i-1].UProc_pte.pteTable[j].entryHI =((0x80000 + j) << 12) | (i << 6);;
            uProcs[i-1].UProc_pte.pteTable[j].entryLO = ALLOFF | DIRTY;
        }

       

        /*fix the last entry's entryHi = 0xBFFFF w/asid*/
        uProcs[i-1].UProc_pte.pteTable[KUSEGSIZE-1].entryHI = (0xBFFFF << 12) | (i << 6);

   

        /*Set up the appropiate three entries in the global segment table
            set KSegOS pointer
            set KUseg2 pointer
            set KUseg3 pointer
            
        */


        /*TESTING FILE MAKES IT TO THIS BREAK POINT*/
     
        /*This line has been tested*/
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
       /*Set the asid*/
        procState.s_asid= (i<<6);
        /*Take the address of the the base that we can allocate then allocate a unique address with 2 pages of memory */
        procState.s_sp = ALLOCATEHERE + ((i-1) * BASESTACKALLOC);
        /*Allive and Well at this point*/
       
        /*Set the process state to be uproc init */
        procState.s_pc = (memaddr) uProcInit;
        procState.s_t9 = (memaddr) uProcInit;
        procState.s_status = ALLOFF | IEON | IMON | TEBITON;
      

        /*Create Process*/
        SYSCALL(SYSCALL1, (int)&procState, 0, 0);

        
    }
    /*for (i=0; i<MAXUPROC; i++){
        P(MasterSema4);
    }
    */
   for (i = 0; i < MAXUPROC; i++)
   {
       /*Gain access of the master semaphore for the 8 processes */
       SYSCALL(SYSCALL4, (int) &masterSem, 0, 0);
   }
   
    /*TERMINATE*/
    SYSCALL(SYSCALL2, 0, 0, 0);

}


void uProcInit()
{

    /*Set a series of local variables*/
    int asid;
    /*Set states for the TLB PROGRAM TRAP AND SYS HANDLER*/
    state_t* newStateTLB;
    state_t* newStatePRG;
    state_t* newStateSYS;
    state_t stateProc;
    /*Set the memory addresses of the TLB PROGRAM TRAP AND SYS HANDLER*/
    memaddr TLBTOP;
    memaddr PROGTOP;
    memaddr SYSTOP;

    /*Figure out who you are? ASID?*/
    asid = getENTRYHI();
    asid = (asid & 0x00000FC0) >> 6;
    
    /*Set up three new areas for Pass Up or Die
        -stack page: to be filled in later
        -PC = address of your Phase 3 handler
        -ASID = your asid value
        -status: all interrupts enabled, local timer enabled, VM ON, Kernel Mode ON
    */
 
    PROGTOP = ALLOCATEHERE + ((asid-1) * BASESTACKALLOC);
    SYSTOP = ALLOCATEHERE + ((asid-1) * BASESTACKALLOC);
    TLBTOP = PROGTOP - PAGESIZE;

    newStateTLB = &(uProcs[asid-1].UProc_NewTrap[TLBTRAP]);
    newStateTLB->s_sp = TLBTOP;
    newStateTLB->s_pc = (memaddr) pager;
    newStateTLB->s_t9 = (memaddr) pager;
    newStateTLB->s_asid = (asid);
    newStateTLB->s_status = ALLOFF | IEON | TEON | VMON | UMOFF;

    newStatePRG = &(uProcs[asid-1].UProc_NewTrap[PROGTRAP]);
    newStatePRG->s_sp = PROGTOP;
    newStatePRG->s_pc = (memaddr) uPgmTrpHandler;
    newStatePRG->s_t9 = (memaddr) uPgmTrpHandler;
    newStatePRG->s_asid = (asid);
    newStatePRG->s_status = ALLOFF | IEON | TEON | VMON | UMOFF;

    newStateSYS = &(uProcs[asid-1].UProc_NewTrap[SYSTRAP]);
    newStateSYS->s_sp = SYSTOP;
    newStateSYS->s_pc = (memaddr) uSysHandler;
    newStateSYS->s_t9 = (memaddr) uSysHandler;
    newStateSYS->s_asid = (asid);
    newStateSYS->s_status = ALLOFF | IEON | TEON | VMON | UMOFF;

   /*Call SYS 5, three times*/
    SYSCALL(SYSCALL5,TLBTRAP,(int) &(uProcs[asid-1].UProc_OldTrap[TLBTRAP]),(int) newStateTLB);
    SYSCALL(SYSCALL5,PROGTRAP,(int) &(uProcs[asid-1].UProc_OldTrap[PROGTRAP]),(int) newStatePRG);
    SYSCALL(SYSCALL5,SYSTRAP,(int) &(uProcs[asid-1].UProc_OldTrap[SYSTRAP]),(int) newStateSYS);



   /*Read the content of the tape devices(asid-1) on the the backing store device (disk0)
       keep reading until the tape block marker (data1) is no longer ENDOFBLOCK
       read block from tape and then write it out to disk0
   */

    /*SYSCALL(SYSCALL4, (int) &mutexArr[0], 0, 0);*/

    device_t* tape;
    device_t* disk;
    devregarea_t * Activedev;
    int buffer;
    int pageNumber;
    unsigned int tapeStatus;
    unsigned int diskStatus;

    Activedev= RAMBASEADDR; 
    pageNumber=0;

    /*Backing Store is at Number 0 !*/
    disk = &(Activedev -> devreg[0]);
    /*The tape is a dynamic number */
    tape = &(Activedev ->devreg[8+(asid-1)]);
    /*Section off some memory for the buffer*/
    buffer = (ROMPAGESTART + (30 * PAGESIZE));
    buffer = buffer + ((asid - 1) * PAGESIZE);


/***********************************************PROBLEM CODE**************************************************/
    /*Atomic operation*/
    
        InterruptsOnOff(FALSE);
		    tape -> d_data0 = buffer;
		    tape -> d_command = DISKREADBLK;
            tapeStatus = SYSCALL(SYSCALL8, TAPEINT, asid-1, 0);
        InterruptsOnOff(TRUE);
    /* loop until whole file has been read */
    /*While Loop is a fucking YIKES */
	while((tape -> d_data1 != EOF) && (tape -> d_data1 != EOT)) {
        /*Debug to check the number of times in the while loop*/
        whi(1);
        /*Atomic operation*/
        InterruptsOnOff(FALSE);
        whi(10);
		    tape -> d_data0 = buffer;
		    tape -> d_command = DISKREADBLK;
        whi(15);
        /*PROBLEM CODE*/   
            tapeStatus = SYSCALL(SYSCALL8, TAPEINT, (asid-1), 0);
        whi(16);
        InterruptsOnOff(TRUE);
        whi(20);
        /*MUTUAL EXCLUSION ON DISK*/
        SYSCALL(SYSCALL4, (int) &mutexArr[0], 0, 0);
        whi(2);
        /*Atomic operation*/
        InterruptsOnOff(FALSE);
        whi(22);
            disk ->d_command = (pageNumber << 8 | 2);
        whi(26);    
            diskStatus = SYSCALL(SYSCALL8, DISKINT, (int) &mutexArr[0], 0);
        whi(28);
        InterruptsOnOff(TRUE);
        whi(4);
        if(diskStatus == READY)
        {
            /*Atomic operation*/
            whi(5);
            InterruptsOnOff(FALSE);
                disk->d_data0 = (ROMPAGESTART + (30 * PAGESIZE)) + ((asid - 1) * PAGESIZE);
                disk->d_command = ((asid - 1) << 8) | 4;
                diskStatus = SYSCALL(SYSCALL8,DISKINT,0,0);
            InterruptsOnOff(TRUE);
        }else{
            SYSCALL(SYSCALL2,0,0,0);
        }
        whi(6);
        /*MUTUAL EXCLUSION ON DISK*/
        SYSCALL(SYSCALL3, (int) &mutexArr[0], 0, 0);

        pageNumber++;
	}
    /*Debug to check if we ever leave the while loop */
    wh(16);
    /*Set up a new state for the user process
        -asid = your asid
        -stack page = last page of KUseg2(0C00.0000)
        -status: all interrupts enabled, local timer enabled, VM ON, User mode ON
        -PC = well known address from the start of KUseg2
    */

    SYSCALL(SYSCALL4, (int) &mutexArr[0], 0, 0);
    stateProc.s_asid = (asid << 6);
    stateProc.s_sp = SEG3;
    stateProc.s_status = ALLOFF | IEON | IMON | TEBITON | UMOFF | TEON | VMON;
    stateProc.s_pc = (memaddr) WELLKNOWNSTARTPROCESS; 
    stateProc.s_t9 = (memaddr) WELLKNOWNSTARTPROCESS;
    SYSCALL(SYSCALL3, (int) &mutexArr[0], 0, 0);
   /*LDST to tihs new state*/
   LDST(&stateProc);

}


/*Test Functions that are deisgned to Test InterruptsOnOff*/
void c(int g){}
void ca(int gg){}

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
