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

if(OffendingLine| (8 << 2) == 1){
    /*Mutli Core is on */


}
if(OffendingLine| (9 << 2) == 1)
{
/*Clock 1 Has an Interrupt */

}
if(OffendingLine| (10 << 2) == 1){
    /*Clock 2 is on */

    
}
if(OffendingLine| (11 << 2) == 1)
{
/*Disk Device is on  */

}
if(OffendingLine| (12 << 2) == 1){
    /*Tape Device is on */

    
}
if(OffendingLine| (13 << 2) == 1)
{
/*Network Device is on */

}
if(OffendingLine| (14 << 2) == 1){
    /*Printer Device is on */

    
}
if(OffendingLine| (15 << 2) == 1)
{
/*Terminal Device is on */

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