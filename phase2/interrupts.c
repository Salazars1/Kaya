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
    int deviceRegisterNumber;
    int temp;  
    int semaphoreaddress; 
    cpu_t timeInterruptOccurs;
    devregarea_t *OffendingDevice;
    
    state_PTR caller;
    caller = (state_t *)INTERRUPTOLDAREA;
    
    
    StoreTime(timeInterruptOccurs);

    OffendingLine = caller->s_cause << 8 | 2;
    

    if ((OffendingLine & MULTICORE) != ZERO)
    {   /*Mutli Core is on */
        PANIC();
    }
    else if ((OffendingLine & CLOCK1) != ZERO)
    {
        /*The process has spent its quantum. Its time to start a new process .*/
        CallScheduler();
        /*Clock 1 Has an Interrupt */
    }
    else if ((OffendingLine & CLOCK2) != ZERO)
    {
        /*Load the clock with 100 Milliseconds*/
        LDIT(PSUEDOCLOCKTIME);
        /*Access the Last clock which is the psuedo clock*/
        semaphoreaddress = (int*) semD[SEMNUM];
        pcb_t * t; 
        while(headBlocked(semadd) != NULL){
            t = removeBlocked(semadd);
            
            insertProcQ(readyQue,t );
            softBlockCount--; 
        }   

        (*semaphoreaddress) = 0; 
        CallScheduler();

    }
    else if ((OffendingLine & DISKDEVICE) != ZERO)
    {
        /*Disk Device is on  */
        lineNumber = DI;
        OffendingDevice = (device_t *)0x1000003c;
    }
    else if ((OffendingLine & TAPEDEVICE) != ZERO)
    {
        /*Tape Device is on */
        lineNumber = TI;
    }

    else if ((OffendingLine & NETWORKDEVICE) != ZERO)
    {
        /*Network Device is on */
        lineNumber = NETWORKI;
    }
    else if ((OffendingLine & PRINTERDEVICE) != ZERO)
    {
        /*Printer Device is on */

        lineNumber = PRINTERI;
    }
    else if ((OffendingLine & TERMINALDEVICE) != ZERO)
    {
        /*Terminal Device is on */
        lineNumber = TERMINALI;
    }
    else
    {
        PANIC();
    }
    
    devicenumber = finddevice(Linenumber);
    /*with Dev Reg and Line number Do literal magic*/

    if(devicenumber == -1){
        PANIC();
    }


    


    /*To Continue Watch Part 2 and 3 of Interrupts but I have alot of questions to ask */

    /*Interrupt has been Handled!*/
    CallScheduler();
}


/*HELPER FUNCTIONS*/

int finddevice(int linenumber)
{
    int i;
    OffendingDevice = (devregarea_t *)DEVPHYS;
    unsigned int map = OffendingDevice->interrupt_dev[linenumber];
    int devn;
    for (i = 0; i < 8; i++)
    {
        if ((map & FIRSTBIT) != ZERO)
        {

            devn = i;
        }
        {
        else

            i = i + 1;
            map << 1;
        }
    }

    return devn;
}

HIDDEN void StoreTime(cpu_t t)
{
    STCK(t);
}

HIDDEN void CallScheduler()
{
    state_t * temp = (state_t *) INTERRUPTOLDAREA
    if (currentProcess == NULL)
    {
        scheduler();
    }
    else
    {
        /*CurrentProcess is Not Null*/
        /*StoreTime(Tim)*/
       /*if the process is still around need to copy its contents over*/
    
     
       CtrlPlusC(temp, currentProcess ->p_s);
   /*Then reinsert the process back onto the ready Queue!*/
       insertProcQ(readyQue, currentProcess);
    /*Call the scheduler to start the next process*/
        scheduler();
    }
}