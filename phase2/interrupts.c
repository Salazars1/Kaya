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


/* Global Variables*/
extern int processCount;
extern int softBlockCount;
extern pcb_t *currentProcess;
extern pcb_t *readyQue;
extern int semD[SEMNUM];

/* Variables for maintaining CPU time*/
extern cpu_t TODStart;

extern void CtrlPlusC(state_PTR oldstate, state_PTR NewState);
HIDDEN int findDevice(int lineNumber);

HIDDEN int testingbaby(int aaaaaa){
    return aaaaaa; 

}

void IOTrapHandler()
{
    testingbaby(1);
    unsigned int offendingLine;
    int lineNumber;
    int devsemnum;
    int devicenumber;
    device_t *  deviceRegisterNumber;
    int* semaphoreAddress;
    int deviceStatus;
    pcb_t * t;
    devregarea_t *OffendingDevice;

    state_PTR caller;
    caller = (state_t *)INTERRUPTOLDAREA;


/*UNSURE*/ 
    offendingLine = caller -> s_cause >> 8; 
    testingbaby(2);

/*finish UNSURE*/

    if ((offendingLine & MULTICORE) != ZERO)
    { /*Mutli Core is on */
        testingbaby(3);
        PANIC();
       
    }
    else if ((offendingLine & CLOCK1) != ZERO)
    {
        /*The process has spent its quantum. Its time to start a new process .*/
        testingbaby(4);
        CallScheduler();
        /*Clock 1 Has an Interrupt */
    }
   
    else if ((offendingLine & CLOCK2) != ZERO)
    {
        /*Load the clock with 100 Milliseconds*/
        LDIT(PSUEDOCLOCKTIME);
        /*Access the Last clock which is the psuedo clock*/
        semaphoreAddress = (int *) &(semD[SEMNUM-1]);
        testingbaby(5);
       
        while(headBlocked(semaphoreAddress) != NULL)
        {
             testingbaby(6);
            t = removeBlocked(semaphoreAddress);
            

            if(t != NULL){
                insertProcQ(&readyQue, t);
                softBlockCount--;
            }
        }
         testingbaby(7);
        (*semaphoreAddress) = 0;
        CallScheduler();
    }
    else if ((offendingLine & DISKDEVICE) != ZERO)
    {
        /*Disk Device is on  */
         testingbaby(8);
        lineNumber = DI;
    }
    else if ((offendingLine & TAPEDEVICE) != ZERO)
    {
        /*Tape Device is on */
         testingbaby(9);
        lineNumber = TI;
    }
    else if ((offendingLine & NETWORKDEVICE) != ZERO)
    {
        /*Network Device is on */
         testingbaby(10);
        lineNumber = NETWORKI;
    }
    else if ((offendingLine & PRINTERDEVICE) != ZERO)
    {
        /*Printer Device is on */
         testingbaby(11);
        lineNumber = PRINTERI;
    }
    else if ((offendingLine & TERMINALDEVICE) != ZERO)
    {
        /*Terminal Device is on */
         testingbaby(12);
        lineNumber = TERMINALI;
    }
    else
    {
        testingbaby(32);
        PANIC();
    }

    devicenumber = finddevice(lineNumber);
    /*with Dev Reg and Line number Do literal magic*/
    devregarea_t *temporary = (devregarea_t *)DEVPHYS;

    if (devicenumber == -1)
    {
        PANIC();
    }
    testingbaby(13);
    /*Need to Determine Device Address and the Device semaphore number*/
    int templinenum;
    /*Offest the Line number*/
    templinenum = lineNumber - 3;

    /* 8 devices per line number*/
    devsemnum = templinenum * 8;
    /*We know which device it is */
    devsemnum = devsemnum + devicenumber;


    device_t * testing;
    int mathishard; 
    int mathishard2; 
    mathishard = 0; 
    mathishard2 = 0; 
    /*The base + 32 (4 words in the device + the size of each register * the register number*/
    /*deviceRegisterNumber = (device_t *)((temporary->rambase + 32) + (devsemnum * DEVREGSIZE));
*/
    mathishard2 = lineNumber - 3; 
    mathishard = mathishard2 * 16; 
    mathishard = mathishard * 8; 
    mathishard2 = devicenumber * 16; 
    mathishard = mathishard + mathishard2; 
    testing = (device_t *) (0x10000050 + mathishard); 
    device_t * devaddrbase; 
    devaddrbase = (device_t *) (0x10000050 + ((lineNumber - 3) * 8*16) + (devicenumber * 16));
    
   /* testing = (device_t *)(0x10000050 + ((lineNumber - 3 ) * (8 * 16) + (devsemnum * DEVREGSIZE)));*/
    
testingbaby(14);
    if (lineNumber == TERMINT)
    {
        /*Terminal*/

        if ((testing->t_transm_status & 0xF) != READY)
        {
                
                /*Acknowledge*/
            testingbaby(15);
                deviceStatus = devaddrbase->t_transm_status;
                testingbaby(1000);
                /*Acknowledge*/
                devaddrbase->t_transm_command = ACK;
                testingbaby(13000);
        }
        else
        {
            
            /*Save the status*/
            deviceStatus = testing->t_recv_status;
            /*Acknowledge*/
            testing->t_recv_command = ACK;
            /*fix the semaphore number for terminal readers sub device */
            devsemnum = devsemnum + DEVPERINT;
        }
    }
    else
    {
        /*Non terminal Interrupt*/
        deviceStatus = testing->d_status;
        /*Acknowledge the interrupt*/
        testing->d_command = ACK;
    }

    
    /*V op */
    semD[devsemnum]++;
    if ((semD[devsemnum]) <= 0)
    {
        t = removeBlocked(semaphoreAddress);
        if (t != NULL)
        {
            t->p_s.s_v0 = deviceStatus;
            insertProcQ(readyQue, t);
            softBlockCount--;
            
        }
    }
    testingbaby(17);
    CallScheduler();
    /*Interrupt has been Handled!*/
}

/*HELPER FUNCTIONS*/

int finddevice(int linenumber)
{
    /*Set some local variables*/
    int i;
    devregarea_t * tOffendingDevice;
    tOffendingDevice = (devregarea_t *) RAMBASEADDR;
    /*make a copy of the bit map */
    unsigned int map = tOffendingDevice->interrupt_dev[linenumber-3];
    int devn;
    testingbaby(19);
    /*8 Total devices to look through */
    for (i = 0; i < TOTALDEVICES; i++)
    {
        /*Bit wise and if the value is not 0 Device is interrupting */
        if ((map & FIRSTBIT) != ZERO)
        {

            devn = i;
            break; 
        }
        
            else
            {

            
                /*Increment both the index and shift the bits 1 */
               
            map >> 1;
        }
    }
    testingbaby(20);
    /*Return the device number*/
    return devn;
}

HIDDEN void CallScheduler()
{
    
    state_t *temp = (state_t *)INTERRUPTOLDAREA;
    
    if (currentProcess == NULL)
    {
        /*Get the next Job */
        scheduler();
    }
    else
    {
        
        
        /*if the process is still around need to copy its contents over*/
        CtrlPlusC(temp, &(currentProcess->p_s));
        insertProcQ(&readyQue, currentProcess);
        /*Load the state back */
        /**LDST(temp);*/
        scheduler();

    }
}