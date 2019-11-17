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


void test()
{
    int i;

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

   /*Process initialization loop (for (i=1; i<MAXUPROC;i++)){
       --initialize stuff
       --SYS 1
    }

    for (i=0; i<MAXUPROC; i++){
        P(MasterSema4);
    }
    
    
    */




}