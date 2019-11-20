/*  PHASE 3
    Written by NICK STONE AND SANTIAGO SALAZAR
    Base comments and some assitance from PROFESSOR MIKEY G 
    Finished on 
*/

#include "../h/const.h"
#include "../h/types.h"

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
   KSegOS.header = TODO:;
   for(i=0;i<KSEGSIZE;i++){
       KSegOS.pteTable[i].entryHI = ((0x20000 + i) << SHIFT_VPN);
       KSegOS.pteTable[i].entryLO = ((0x20000 + i) << SHIFT_VPN) | DIRTY | GLOBAL | VALID;
    }

   /*kuSeg3 page table
        32 entries
            entryHI= 0xC0000+i
            entryLO= Dirty, Global
    */
   kuSeg3.header = TODO:;
   for(i=0;i<KUSEGSIZE;i++){
       KSegOS.pteTable[i].entryHI = ((0xC0000 + i) << TODO:);
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

        /*KUseg2 page table
            -32 entries:
                entryHI=0x80000+i w/asid
                entryLO = Dirty
        */
        for(j = 0; j < KUSEGSIZE; j++)
        {
            uProcs[i-1].UProc_pte.pteTable[j].entryHI = 0x80000 + j;
            uProcs[i-1].UProc_pte.pteTable[j].entryLO = ALLOFF | DIRTY;
        }

        /*fix the last entry's entryHi = 0xBFFFF w/asid*/
        uProcs[i-1].uProc_pte.pteTable[KUSEGSIZE-1].entryHI = TODO:

        /*Set up the appropiate three entries in the global segment table
            set KSegOS pointer
            set KUseg2 pointer
            set KUseg3 pointer*/
        
        segTable = (segTbl_t *) (0x20000500 + (i * 0x20000500));
        segTable->ksegOS= &KSegOS;
        segTable->kuseg2= FIXME:
        segTable->kuseg3= &kuSeg3;

        /*Set up an initial state for a user process
            -asid =i
            -stack page = tp be filled in later
            -PC = uProcInit
            -status: all interrupts enabled, local timer enabled, VM off, kernel mode on
        */

        procState.s_asid=i FIXME: need to do some bit shift?;
        procState.s_SP = FIXME:
        procState.s_pc = (memaddr) UPROCSETUP;
        procState.s_t9 = (memaddr) UPROCSETUP;
        procState.s_status = ALLOFF | IEON | IMON | TEBITON;
        
        /*SYS 1 (vvv)*/
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