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
#include "../e/scheduler.e"
#include "../e/pcb.e"
#include "../e/asl.e"
#include "/usr/local/include/umps2/umps/libumps.e"

/* Global Variables*/
extern int processCount;
extern int softBlockCount;
extern pcb_t *currentProcess;
extern pcb_t *readyQue;
extern int semD[SEMNUM];

/* Variables for maintaining CPU time*/
extern cpu_t currentTOD;
extern cpu_t TODStart;


extern void CtrlPlusC(state_PTR oldstate, state_PTR NewState);
HIDDEN int findDevice(int lineNumber);


void IOTrapHandler()
{

    unsigned int offendingLine;    
    int lineNumber;
    int devicenumber;

    cpu_t timeInterruptOccurs;
    devregarea_t *OffendingDevice;
    
    state_PTR caller;
    caller = (state_t *)INTERRUPTOLDAREA;
    
    
    StoreTime(timeInterruptOccurs);

    OffendingLine = caller->s_cause << 8 | 2;
    

    if ((OffendingLine & MULTICORE) == MULTICORE)
    {   /*Mutli Core is on */
        PANIC();
    }
    else if ((OffendingLine & CLOCK1) == CLOCK1)
    {
        /*Clock 1 Has an Interrupt */
    }
    else if ((OffendingLine & CLOCK2) == CLOCK2)
    {
        /*Clock 2 is on */
    }
    else if ((OffendingLine & DISKDEVICE) == DISKDEVICE)
    {
        /*Disk Device is on  */
        lineNumber = DI;
        OffendingDevice = (device_t *)0x1000003c;
    }
    else if ((OffendingLine & TAPEDEVICE) == TAPEDEVICE)
    {
        /*Tape Device is on */
        lineNumber = TI;
    }

    else if ((OffendingLine & NETWORKDEVICE) == NETWORKDEVICE)
    {
        /*Network Device is on */
        lineNumber = NETWORKI;
    }
    else if ((OffendingLine & PRINTERDEVICE) == PRINTERDEVICE)
    {
        /*Printer Device is on */

        lineNumber = PRINTERI;
    }
    else if ((OffendingLine & TERMINALDEVICE) == TERMINALDEVICE)
    {
        /*Terminal Device is on */
        lineNumber = TERMINALI;
    }
    else
    {
        PANIC();
    }
    
    int devn;

    devn = finddevice(Linenumber);
    /*with Dev Reg and Line number Do literal magic*/

    if(devn == -1){
        PANIC();
    }

    

    /*To Continue Watch Part 2 and 3 of Interrupts but I have alot of questions to ask */

    /*Interrupt has been Handled!*/
    callscheduler();
}
/**
 * Given Line Number fine the Device Number knowing that It is also a bit map ;
*/
/*This Device belongs to some memory WHere is unknown*/

/**
 * Need to completely Refactor this shit show that I have right now 
 * 
 * We need to build helper functions that should probably do the following
 * Function: Finish the program and call the scheduler
 * Function: Find the device number given the line number 
 * Function: Store time 
 * Function: Copy State? 
 * Function: 
 * 
 *
 * 
 * 
 * 
 * 
*/

/*HELPER FUNCTIONS*/

int finddevice(int linenumber)
{
    int i;
    OffendingDevice = (devregarea_t *)DEVPHYS;
    unsigned int bit = OffendingDevice->interrupt_dev[linenumber];
    for (i = 0; i < 8; i++)
    {
        if ((bit & 0x00000001) == 0x00000001)
        {

            devicenumber = i;
        }
        else
        {

            i = i + 1;
            bit << 1;
        }
    }

    return devicenumber;
}

HIDDEN void StoreTime(cpu_t t)
{
    STCK(t);
}

HIDDEN void CallScheduler()
{

    if (currentProcess == NULL)
    {
        scheduler();
    }
    else
    {
        /*CurrentProcess is Not Null*/
        /*StoreTime(Tim)*/
        cpu_t timespent;
        StoreTime(timespent);

        scheduler();
    }
}