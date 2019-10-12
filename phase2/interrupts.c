/*  PHASE 2
    Written by NICK STONE AND SANTIAGO SALAZAR
    Base code and Comments from PROFESSOR MIKEY G 
    Finished on 
*/

/*********************************************************************************************
                            Module Comment Section

**********************************************************************************************/

#include "../h/const.h"
#include "../h/types.h"
#include "../e/initial.e"
#include "../e/pcb.e"


extern void CtrlPlusC(state_PTR oldstate, state_PTR NewState);


void IOTrapHandler(){

unsigned int OffendingLine;
int Linenumber; 
int devicenumber; 
cpu_t timeInterruptOccurs; 
devregarea_t * OffendingDevice;
state_PTR caller; 
caller = (state_t *) INTERRUPTOLDAREA;
OffendingLine = caller -> s_cause << 8 | 2; 
StoreTime(timeInterruptOccurs);



if((OffendingLine &  MULTICORE) == MULTICORE ){
    /*Mutli Core is on */
    PANIC();
    HALT();

}
else if((OffendingLine &  CLOCK1) == CLOCK1 )
{
/*Clock 1 Has an Interrupt */

}
else if((OffendingLine & CLOCK2) == CLOCK2 ){
    /*Clock 2 is on */

    
}
else if((OffendingLine & DISKDEVICE) == DISKDEVICE )
{
/*Disk Device is on  */
    Linenumber = DI; 
    OffendingDevice = (device_t *) 0x1000003c;


}
else if((OffendingLine & TAPEDEVICE)  == TAPEDEVICE ){
    /*Tape Device is on */
    Linenumber = TI; 

}
    
else if((OffendingLine & NETWORKDEVICE) == NETWORKDEVICE )
{
/*Network Device is on */
    Linenumber = NETWORKI; 

}
else if((OffendingLine & PRINTERDEVICE) == PRINTERDEVICE ){
    /*Printer Device is on */

    Linenumber = PRINTERI; 
}
else if((OffendingLine & TERMINALDEVICE) == TERMINALDEVICE )
{
/*Terminal Device is on */
    Linenumber = TERMINALI; 
}

/**
 * Given Line Number fine the Device Number knowing that It is also a bit map ;
*/
/*This Device belongs to some memory WHere is unknown*/


int i; 
OffendingDevice = (devregarea_t *)DEVPHYS;
unsigned int bit = OffendingDevice -> interrupt_dev[Linenumber];
for(i = 0; i < 8; i++ ){
    if((bit & 0x00000001) == 0x00000001){

        devicenumber = i; 



    }
    else{ 

        i = i + 1; 
        bit << 1; 
        }

    }






    CallScehduluer();
}

HIDDEN void StoreTime(cpu_t t){
    STCK(t);
}


HIDDEN void CallScheduluer(){



    scheduler();




}