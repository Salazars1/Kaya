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
#include "../e/asl.e"
#include "../e/pcb.e"
#include "../e/initial.e"
#include "../e/interrupts.e"
#include "../e/exceptions.e"
#include "../e/scheduler.e"



#include "/usr/local/include/umps2/umps/libumps.e"


/* Global Variables from initial.e*/
extern int processCount;
extern int softBlockCount;
extern pcb_t *currentProcess;
extern pcb_t *readyQue;
extern int semD[SEMNUM];
/*We want to use the copy state fucntion from exceptions*/
extern void CtrlPlusC(state_PTR oldstate, state_PTR NewState);
/*2 additional functions to help compute the device number and call the scheduler*/
int finddevice(int linenumber);
void CallScheduler();
void IOTrapHandler()
{
    /*Line number bit map that is in the cause register of the state stored in interrupt old area*/
    unsigned int offendingLine;
    /*Need to Determine Device Address and the Device semaphore number*/
    int templinenum;
    int lineNumber;
    int devsemnum;
    int devicenumber;
    /*V operation sempahore variables */
    int * semad;
    int* semaphoreAddress;
    /*Store the device status to place in v0*/
    int deviceStatus;
    /*Another timing variable*/
    pcb_t * newprocess;
    state_PTR caller;
    /*Get the state of the offending interrupt*/
    caller = (state_t *)INTERRUPTOLDAREA;
    /*Shift 8 since we only care about bits 8-15*/
    offendingLine = caller ->s_cause >> 8;
    if ((offendingLine & MULTICORE) != ZERO)
    { /*Mutli Core is on */
        PANIC();
    }
    else if ((offendingLine & CLOCK1) != ZERO)
    {
        /*The process has spent its quantum. Its time to start a new process .*/
        CallScheduler();
        /*Clock 1 Has an Interrupt */
    }

    else if ((offendingLine & CLOCK2) != ZERO)
    {
        /*Access the Last clock which is the psuedo clock*/
        semaphoreAddress = (int *) &(semD[SEMNUM-1]);
       /*Free all of the processes that are currently blocked and put them onto the ready queue*/
        while(headBlocked(semaphoreAddress) != NULL)
        {
            /*Remove from the blocked list*/
            newprocess = removeBlocked(semaphoreAddress);
            /*if not null then we put that bitch back onto the ready queue*/
            if(newprocess != NULL){
                insertProcQ(&readyQue, newprocess);
                /*One less softblock process */
                softBlockCount = softBlockCount - 1;
            }
        }
         /*Set the semaphore back to 0*/
        *semaphoreAddress = 0;
        /*Load the clock with 100 Milliseconds*/
        LDIT(PSUEDOCLOCKTIME);
        CallScheduler();
    }
    else if ((offendingLine & DISKDEVICE) != ZERO)
    {
        /*Disk Device is on  */
        lineNumber = DI;
    }
    else if ((offendingLine & TAPEDEVICE) != ZERO)
    {
        /*Tape Device is on */
        lineNumber = TI;
    }
    else if ((offendingLine & NETWORKDEVICE) != ZERO)
    {
        /*Network Device is on */

        lineNumber = NETWORKI;
    }
    else if ((offendingLine & PRINTERDEVICE) != ZERO)
    {

        /*Printer Device is on */

        lineNumber = PRINTERI;
    }
    else if ((offendingLine & TERMINALDEVICE) != ZERO)
    {
        /*Terminal Device is on */
        lineNumber = TERMINALI;
    }
    /*Not recognized so go ahead and panic for me*/
    else
    {
        PANIC();
    }

    /*Call the helper function since we have the line number and need to find the device number*/
    devicenumber = finddevice(lineNumber);
    /*Offest the Line number*/
    templinenum = lineNumber - DEVWOSEM;
    /* 8 devices per line number*/
    devsemnum = templinenum * DEVPERINT;
    /*We know which device it is */
    devsemnum = devsemnum + devicenumber;
    device_t * OffendingDeviceRegister; 
    OffendingDeviceRegister = (device_t *)(0x10000050 + (templinenum * 0x80) + (devicenumber * 0x10));
    /*If the line number is a terminal which is why we dont decrement line number by 3 and assign a new variable!*/
    if (lineNumber == TERMINT)
    {
        /*Terminal*/
        if ((OffendingDeviceRegister->t_transm_status & 0xF) != READY)
        {
             /*Acknowledge*/
                OffendingDeviceRegister->t_transm_command = ACK;
                /*Set the device status*/
                deviceStatus = OffendingDeviceRegister->t_transm_status;
               
        }
        else
        {
             /*Acknowledge*/
            OffendingDeviceRegister->t_recv_command = ACK;
            /*Semaphore number + 8 */
            devsemnum = devsemnum + DEVPERINT;
            /*Save the status*/
            deviceStatus = OffendingDeviceRegister->t_recv_status;
           
            /*fix the semaphore number for terminal readers sub device */
        }
    }
    /*Not a terminal pretty straight forward*/
    else
    {
        /*Acknowledge the interrupt*/
        OffendingDeviceRegister->d_command = ACK;
        /*Non terminal Interrupt*/
        deviceStatus = OffendingDeviceRegister->d_status;
        
    }
    /*Get the semaphore for the device causing the interrupt*/
    semad =&(semD[devsemnum]);
    /*Increment by 1 */
    (*semad)= (*semad) +1;
    if ((*semad) <= 0)
    {   /*Remove one from the blocked list and if that is not null*/
        newprocess = removeBlocked(semad);
        newprocess-> p_s.s_v0 = deviceStatus;
        if (newprocess != NULL)
        {
            /*Set the status in the v0 register decrement the softblock count and insert it onto the ready queue*/
            softBlockCount = softBlockCount - 1;
            insertProcQ(&readyQue, newprocess);
        }
    }
    CallScheduler();
    /*Interrupt has been Handled!*/
    PANIC();
}

/*HELPER FUNCTIONS*/
/**
 * Take in the line number of the interrupt that is offending. We will then bit shift until we find the first device causing
 * The interrupt
 * parameter: Int Line number of interrupt
 * Return type: Int of the offending device number
*/
int finddevice(int linenumber)
{
    /*Set some local variables*/
    /*For loop counter*/
    int i;
    /*area that is causing the interrupt that has the map of the device*/
    devregarea_t * tOffendingDevice;
    /*tt is to track the line number - 3 */
    int ProperLineNumber;
    /*The bit map of the device bit map and a bit map only containning the first bit*/
    unsigned int LineBitmap;
    unsigned int  BitMapActive;
    /*Device number*/
    int offendingdevicenumber;
    /*WE know that the line number - 3 DEVNOSEM*/
    ProperLineNumber = linenumber -3;
   /*SEt this to be the RAMBASEADDR*/
    tOffendingDevice = (devregarea_t *) RAMBASEADDR;
    /*make a copy of the bit map */
    LineBitmap = tOffendingDevice->interrupt_dev[ProperLineNumber];
    /*Only care about the first bit */
    BitMapActive = FIRSTBIT;
    /*8 Total devices to look through */
    for (i = 0; i < TOTALDEVICES; i++)
    {
        /*Bit wise and if the value is not 0 Device is interrupting */
        if ((LineBitmap & BitMapActive) == BitMapActive)
        {
            /*Device number is equal to the index and we are done looping */
            offendingdevicenumber = i;
            break;
        }
        /* shift the map to the right 1 to check the next device */
        LineBitmap = LineBitmap >> 1;   
    }
    /*Return the device number*/
    return offendingdevicenumber;
}

/*Function in charge of either putting the current process back on the ready queue and calling the scheduler
* Or just going to call the scheduler
* Parameters: None
* Return: void
*/
void CallScheduler()
{
    /*set a temp state to point to the interrupt old area*/
    state_t *temp;
    temp =  (state_t *)INTERRUPTOLDAREA;
    /*If the current process is null we need to put it back onto the ready queue*/
    if (currentProcess != NULL)
    {
         /*if the process is still around need to copy its contents over*/
        CtrlPlusC(temp, &(currentProcess->p_s));
        insertProcQ(&readyQue, currentProcess);
        /*Load the state back */
        /**LDST(temp);*/
        scheduler();
    }
    /*No Current Process go ahead and call the scheduler for the next process*/
    else
    {
        /*No extra work need to be done just go ahead and call scheduler since there is no currentProcess*/
      scheduler();
    }
}

