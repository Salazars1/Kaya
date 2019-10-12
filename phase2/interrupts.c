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
OffendingLine = CALLERADDRESS -> s_cause << 8 | 2; 
int Linenumber; 
int devicenumber; 
cpu_t timeInterruptOccurs; 


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
devicearea_t * OffendingDevice = ; 

int i; 
unsigned int bit = interrupt_dev[Linenumber];
for(i = 0; i < ){
    if(){



    }

else{ 

i = i + 1; 
/*BitSHift*/

}
}




HIDDEN void StoreTime(cpu_t t){
    STCK(t);
}


}