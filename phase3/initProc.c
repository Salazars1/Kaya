/*  PHASE 3
    Written by NICK STONE AND SANTIAGO SALAZAR
    Base comments and some assitance from PROFESSOR MIKEY G 
    Finished on 
*/

#include "../h/const.h"
#include "../h/types.h"

#include "../e/initProc.e"
#include "../e/vmIOsupport.e"

pteOS_t KSegOS; /*OS page table*/
pte_t kuSeg3;   /*Seg3 page table*/
swap_t swapPool[SWAPPOOLSIZE];

int swapSem;
int mutexArr[SEMNUM];
int masterSem;

uProc_t uProcs[MAXUPROC];

void test()
{
    int i;
    int j;
    state_t procState;
    segTable_t* segTable;

    /*KSegOS page table
        64 entries
            entryHI= 0x20000+i
            entryLO= 0x20000+i w/Dirty, Global, Valid
    */
   KSegOS.header = (0x2A<<24)|KSEGSIZE;
   for(i=0;i<KSEGSIZE;i++){
       KSegOS.pteTable[i].entryHI = ((0x20000 + i) << 12);
       KSegOS.pteTable[i].entryLO = ((0x20000 + i) << 12) | DIRTY | GLOBAL | VALID;
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
       swapPool[i].sw_asid = -1;
       swapPool[i].sw_segNum = 0;
       swapPool[i].sw_pgNum = 0;
       swapPool[i].sw_pte = NULL;
    }

    /*swap pool sema4
        init to 1
    */
    swapSem = 1;

    /*an array of sempahores: one for each interrupting device
        -init to 1
    */
    for(i=0; i< SEMNUM; i++){
        mutexArr[i] = 1;
    }
   
    /*MasterSema4
        -init to 0
    */
    masterSem = 0;

   /*Process initialization loop (for (i=1; i<MAXUPROC+1;i++)){
       --initialize stuff
       --SYS 1
    }   
    */
    for(i =1; i<MAXUPROC;i++){
        /* i becomes the ASID (processID)*/
        uProcs[i-1].UProc_pte.header = (0x2A<<24)|KUSEGSIZE;

        /*KUseg2 page table
            -32 entries:
                entryHI=0x80000+i w/asid
                entryLO = Dirty
        */
        for(j = 0; j < KUSEGSIZE; j++)
        {
            uProcs[i-1].UProc_pte.pteTable[j].entryHI =((0x80000 + j) << 12) | (i << 6);;
            uProcs[i-1].UProc_pte.pteTable[j].entryLO = ALLOFF | DIRTY;
        }

        /*fix the last entry's entryHi = 0xBFFFF w/asid*/
        uProcs[i-1].uProc_pte.pteTable[KUSEGSIZE-1].entryHI = (0xBFFFF << 12) | (i << 6);

        /*Set up the appropiate three entries in the global segment table
            set KSegOS pointer
            set KUseg2 pointer
            set KUseg3 pointer*/
        
        segTable = (segTbl_t *) (0x20000500 + (i * 0x20000500));
        segTable->ksegOS= &KSegOS;
        segTable->kuseg2= &(uProcs[i-1].UProc_pte);
        segTable->kuseg3= &kuSeg3;

        /*Set up an initial state for a user process
            -asid =i
            -stack page = tp be filled in later
            -PC = uProcInit
            -status: all interrupts enabled, local timer enabled, VM off, kernel mode on
        */

        procState.s_asid= (i<<6);
        procState.s_sp = FIXME:; we need three stack pages per proceass (TLB, SYS, )//////////////////
        procState.s_pc = (memaddr) uProcInit;
        procState.s_t9 = (memaddr) uProcInit;
        procState.s_status = ALLOFF | IEON | IMON | TEBITON;
        
        /*SYS 1 (v)*/
        SYSCALL(SYSCALL1, (int)&procState, 0, 0);
    }

    /*for (i=0; i<MAXUPROC; i++){
        P(MasterSema4);
    }
    */
   for (i = 0; i < MAXUPROC; i++)
   {
       SYSCALL(SYSCALL4, (int) &masterSem, 0, 0);
   }
   
    /*SYS2*/
    SYSCALL(SYSCALL2, 0, 0, 0);
}


void uProcInit()
{
    int asid;
    state_t* newStateTLB;
    state_t* newStatePRG;
    state_t* newStateSYS;

    state_t stateProc;

    /*Figure out who you are? ASID?*/
    asid = getASID();

    /*Set up three new areas for Pass Up or Die
        -stack page: to be filled in later
        -PC = address of your Phase 3 handler
        -ASID = your asid value
        -status: all interrupts enabled, local timer enabled, VM ON, Kernel Mode ON
    */
    newStateTLB = &(uProcs[asid-1].UProc_NewTrap[TLBTRAP]);
    newStateTLB->s_sp = FIXME:;
    newStateTLB->s_pc = (memaddr) pager;
    newStateTLB->s_t9 = (memaddr) pager;
    newStateTLB->s_asid = (asid << 6);
    newStateTLB->s_status = ALLOFF | IEON | TEON | VMON | UMOFF;

    newStatePRG = &(uProcs[asid-1].UProc_NewTrap[PROGTRAP]);
    newStatePRG->s_sp = FIXME:;
    newStatePRG->s_pc = (memaddr) uPgmTrpHandler;
    newStatePRG->s_t9 = (memaddr) uPgmTrpHandler;
    newStatePRG->s_asid = (asid << 6);
    newStatePRG->s_status = ALLOFF | IEON | TEON | VMON | UMOFF;

    newStateSYS = &(uProcs[asid-1].UProc_NewTrap[SYSTRAP]);
    newStateSYS->s_sp = FIXME:;
    newStateSYS->s_pc = (memaddr) uSysHandler;
    newStateSYS->s_t9 = (memaddr) uSysHandler;
    newStateSYS->s_asid = (asid << 6);
    newStateSYS->s_status = ALLOFF | IEON | TEON | VMON | UMOFF;

   /*Call SYS 5, three times*/
    SYSCALL(SYSCALL5,TLBTRAP,(int) &(uProcs[asid-1].UProc_OldTrap[TLBTRAP]),(int) newStateTLB);
    SYSCALL(SYSCALL5,PROGTRAP,(int) &(uProcs[asid-1].UProc_OldTrap[PROGTRAP]),(int) newStatePRG);
    SYSCALL(SYSCALL5,SYSTRAP,(int) &(uProcs[asid-1].UProc_OldTrap[SYSTRAP]),(int) newStateSYS);

   /*Read the content of the tape devices(asid-1) on the the backing store device (disk0)
       keep reading until the tape block marker (data1) is no longer ENDOFBLOCK
       read block from tape and then write it out to disk0
   */

    device_t* tape;
    device_t* disk;
    int buffer;
    int pageNumber;
    int tapeStatus;
    int diskStatus;


    pageNumber=0;
    tape = TODO:;
    disk = TODO:;
    buffer = (ROMPAGESTART + (30 * PAGESIZE))+ ((asid - 1) * PAGESIZE);

    /* loop until whole file has been read */
	while((tape -> d_data1 != EOF) && (tape -> d_data1 != EOT)) {

        /*Atomic operation*/
        InterruptsOnOff(FALSE);
		    tape -> d_data0 = buffer;
		    tape -> d_command = DISKREADBLK;
            tapeStatus = SYSCALL(SYSCALL8, TAPEINT, (asid-1), 0);
        InterruptsOnOff(TRUE);

        /*MUTUAL EXCLUSION ON DISK*/
        SYSCALL(SYSCALL4, (int) &mutexArr[0], 0, 0);

        /*Atomic operation*/
        InterruptsOnOff(FALSE);
            disk ->d_command = (pageNumber << 8 | 2);
            diskStatus = SYSCALL(SYSCALL8, DISKINT, 0, 0);
        InterruptsOnOff(TRUE);

        if(diskStatus == READY)
        {
            /*Atomic operation*/
            InterruptsOnOff(FALSE);
                disk->d_data0 = TODO:;
                disk->d_command =TODO:;

                diskStatus = SYSCALL(SYSCALL8,DISKINT,0,0);
            InterruptsOnOff(TRUE);
        }else{
            SYSCALL(SYSCALL2,0,0,0);
        }

        /*MUTUAL EXCLUSION ON DISK*/
        SYSCALL(SYSCALL3, (int) &mutexArr[0], 0, 0);

        pageNumber++;
	}
    
    /*Set up a new state for the user process
        -asid = your asid
        -stack page = last page of KUseg2(0C00.0000)
        -status: all interrupts enabled, local timer enabled, VM ON, User mode ON
        -PC = well known address from the start of KUseg2
    */

    stateProc.s_asid = (asid << 6);
    stateProc.s_sp = FIXME:;
    stateProc.s_status = ALLOFF | IEON | IMON | TEBITON | UMOFF | TEON | VMON;
    stateProc.s_pc = (memaddr) WELLKNOWNSTARTPROCESS; 
    stateProc.s_t9 = (memaddr) WELLKNOWNSTARTPROCESS;

   /*LDST to tihs new state*/
   LDST(&stateProc);
}


 void InterruptsOnOff(int IOturned)
{
    int status = getSTATUS();

    if(!IOturned)
    {
        status = (status & 0xFFFFFFFE);
    }
    else
    {
        status = (status | 0x1);
    }
    setSTATUS(status);
}
