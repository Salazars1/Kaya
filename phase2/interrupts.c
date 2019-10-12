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


void IOTrapHandler(state_PTR caller){

unsigned int OffendingLine = caller -> s_cause; 
int Linenumber; 
int devicenumber; 
cpu_t timeInterruptOccurs; 





if(OffendingLine &  MutliCore == MultiCore ){
    /*Mutli Core is on */


}
else if(OffendingLine &  Clock1 == Clock1 )
{
/*Clock 1 Has an Interrupt */

}
else if(OffendingLine & Clock2 == Clock2 ){
    /*Clock 2 is on */

    
}
else if(OffendingLine & DiskDevice == DiskDevice )
{
/*Disk Device is on  */

}
else if(OffendingLine & TapeDevice  == TapeDevice ){
    /*Tape Device is on */

}
    
else if(OffendingLine & NetworkDevice == NetworkDevice )
{
/*Network Device is on */

}
else if(OffendingLine & PrinterDevice == PrinterDevice ){
    /*Printer Device is on */

    
}
else if(OffendingLine & TerminalDevice == TerminalDevice )
{
/*Terminal Device is on */

}
else{
/*We Interrupted with No interrupt Being on Stop the Program */
HALT();
}





/**
 * Step 1 in the Interrupt handler is to Find which Line is causing the Interrupt. 
 * 
 * Line 0 - Multi core - Doesnt matter
 * Line 1 & 2 - Clocks
 * Line 3 Disk Device 
 * Line 4 Tape Device 
 * Line 5 Network Device
 * Line 6 Printer Device 
 * Line 7 Terminal Device 
 * 
 * To Determine the Cause Register Bit Manipulation must be done on an 8 Bit field
 * 
 * 
 * Want to find which bit has a 1 (Meaning On) In the lowest to highest priority 
 * Little Endian so from Right to Left (Backwards) 8 - 15
 * 
 * Line 1 and 2 Handled DIfferently
 * 
 * Devices 3-7 
 * How to handle this
 * 
 * 
 * Given the Line Number and Device Number we can determine that
 * 
 * 
 * 
*/
    
}