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
int tes(int c){
    return c; 
}

extern void addokbuf(char *strp);

void IOTrapHandler()
{
    /*addokbuf("\n INTERRUPTS HAVE STARTED \n");*/
    int b = 1; 
    testingbaby(b);
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
    /*addokbuf("Called and the rest of the variables are set \n");*/

/*UNSURE 
    offendingLine = caller -> s_cause >> 8; 
    testingbaby(2);

finish UNSURE*/


    offendingLine = ((caller ->s_cause) & IPAREA) >> 8;
   /*addokbuf("check the test suite to see the offending line \n");*/
    /*offendingLine = caller ->s_cause >> 8;*/ 

   /* test(offendingLine);*/



    if ((offendingLine & MULTICORE) != ZERO)
    { /*Mutli Core is on */
       addokbuf("Multi Core go ahead and panic \n");
        testingbaby(3);
        PANIC();
       
    }
    else if ((offendingLine & CLOCK1) != ZERO)
    {
           addokbuf("QUantum is up CLock 1 call scheduler \n");
        /*The process has spent its quantum. Its time to start a new process .*/
        testingbaby(4);
        CallScheduler();
        /*Clock 1 Has an Interrupt */
    }
   
    else if ((offendingLine & CLOCK2) != ZERO)
    {
        /*Load the clock with 100 Milliseconds*/
           addokbuf("Psuedo Clock\n");
        LDIT(PSUEDOCLOCKTIME);
        /*Access the Last clock which is the psuedo clock*/
        semaphoreAddress = (int *) &(semD[SEMNUM-1]);
        testingbaby(5);
       
        while(headBlocked(semaphoreAddress) != NULL)
        {
               /*addokbuf("Headblocked is running \n");*/
             testingbaby(6);
            t = removeBlocked(semaphoreAddress);
               /*addokbuf("Remove the process from the blocked \n");*/

            if(t != NULL){
                   /*addokbuf("new process is not null \n");*/
                insertProcQ(&readyQue, t);
                softBlockCount--;
            }
        }
         testingbaby(7);
        (*semaphoreAddress) = 0;
        addokbuf("Reset the semaphore address and call scheduler \n");
        CallScheduler();
    }
    else if ((offendingLine & DISKDEVICE) != ZERO)
    {
           addokbuf("The offending line is 3 or a disk device \n");
        /*Disk Device is on  */
         testingbaby(8);
        lineNumber = DI;
    }
    else if ((offendingLine & TAPEDEVICE) != ZERO)
    {
        /*Tape Device is on */
           addokbuf("The offending line is a tape device \n");
         testingbaby(9);
        lineNumber = TI;
    }
    else if ((offendingLine & NETWORKDEVICE) != ZERO)
    {
        /*Network Device is on */
           addokbuf("THe offending line is a network device 5 \n");
         testingbaby(10);
        lineNumber = NETWORKI;
    }
    else if ((offendingLine & PRINTERDEVICE) != ZERO)
    {
           addokbuf("The offending linne is printer device 6  \n");
        /*Printer Device is on */
         testingbaby(11);
        lineNumber = PRINTERI;
    }
    else if ((offendingLine & TERMINALDEVICE) != ZERO)
    {
           addokbuf("the offending line is a terminal device -> FUck  \n");
        /*Terminal Device is on */
         testingbaby(12);
        lineNumber = TERMINALI;
    }
    else
    {
           addokbuf("Not a known line number PANIC \n");
        testingbaby(32);
        PANIC();
    }

       /*addokbuf("Geting the device number \n");*/
    devicenumber = finddevice(lineNumber);
  /*  tes(devicenumber);*/
       /*addokbuf("Check the device number in a debug fun \n");*/
    /*with Dev Reg and Line number Do literal magic*/
    devregarea_t *temporary = (devregarea_t *)DEVPHYS;

    if (devicenumber == -1)
    {
           /*addokbuf("Device number is -1 panic  \n");*/
        testingbaby(10200202);
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
   /*addokbuf("The math is being computated for the device number and device base  \n");*/
    mathishard2 = lineNumber - 3; 
    mathishard = mathishard2 * 16; 
    mathishard = mathishard * 8; 
    mathishard2 = devicenumber * 16; 
    mathishard = mathishard + mathishard2; 
    testing = (device_t *) (0x10000050 + mathishard); 
    device_t * devaddrbase; 
    devaddrbase = (device_t *) (0x10000050 + ((lineNumber - 3) * 8*16) + (devicenumber * 16));
    
   /* testing = (device_t *)(0x10000050 + ((lineNumber - 3 ) * (8 * 16) + (devsemnum * DEVREGSIZE)));*/
    
    devsemnum = lineNumber -3; 
    devsemnum = devsemnum * 8; 
    devsemnum = devsemnum + devicenumber; 
testingbaby(14);
    if (lineNumber == TERMINT)
    {

           /*addokbuf("We got a terminal \n");*/
        /*Terminal*/

        if ((testing->t_transm_status & 0x0F) != READY)
        {
                   /*addokbuf("We are transmitting \n");*/
                /*Acknowledge*/
            testingbaby(15);
                deviceStatus = devaddrbase->t_transm_status;
                testingbaby(23);
                /*Acknowledge*/
                devaddrbase->t_transm_command = ACK;

                testingbaby(34);
        }
        else
        {
               /*addokbuf("We are recieving  \n");*/
            /*Save the status*/
            devsemnum += 1; 
          
            deviceStatus = testing->t_recv_status;
            /*Acknowledge*/
            testing->t_recv_command = ACK;
            /*fix the semaphore number for terminal readers sub device */
           /* devsemnum = devsemnum + DEVPERINT;*/
        }
    }
    else
    {
           /*addokbuf("Not a terminal \n");*/
        /*Non terminal Interrupt*/
        deviceStatus = testing->d_status;
        /*Acknowledge the interrupt*/
        testing->d_command = ACK;
    }

    testingbaby(43);
    /*V op */
       /*addokbuf("Playing with semaphores go ahead and check these in the debug functions \n");*/
    int * semad; 

    semad =&(semD[devsemnum]);

    
    (*semad)--; 
   /* tes(*semad);*/
    
    testingbaby(3);
    if (*semad <= 0)
    {
           addokbuf("Value is less than 0 \n");
        t = removeBlocked(*semad);
        if (t != NULL)
        {
               /*addokbuf("t is a process that was on the blocked queue \n");*/
            t->p_semAdd = NULL;
            t-> p_s.s_v0 = deviceStatus; 
            insertProcQ(&readyQue, t);
            softBlockCount--;
            
        }
        else{
               /*addokbuf("Process is null halt \n");*/
            HALT();
        }
    }
    testingbaby(17);
       /*addokbuf("Call scheduler \n");*/
    CallScheduler();
    /*Interrupt has been Handled!*/
}

/*HELPER FUNCTIONS*/

int finddevice(int linenumber)
{
       /*addokbuf("Finding this fucking device number  \n");*/
    /*Set some local variables*/
    int i;
    devregarea_t * tOffendingDevice;
    tOffendingDevice = (devregarea_t *) RAMBASEADDR;
    /*make a copy of the bit map */
    unsigned int map = tOffendingDevice->interrupt_dev[linenumber-3];
    unsigned int  t = FIRSTBIT; 
    int devn;
    testingbaby(19);
    /*8 Total devices to look through */
    for (i = 0; i < TOTALDEVICES; i++)
    {
        /*Bit wise and if the value is not 0 Device is interrupting */
        if ((map & t) != ZERO)
        {

            devn = i;
            break; 
        }
        
            else
            {

            
                /*Increment both the index and shift the bits 1 */
               
            t << 1;
        }
    }
    testingbaby(20);
    /*Return the device number*/
    return devn;
}

HIDDEN void CallScheduler()
{
       /*addokbuf("Calling the shceduler has started \n");*/
    state_t *temp;
    temp =  (state_t *)INTERRUPTOLDAREA;
    
    if (currentProcess != NULL)
    {
           /*addokbuf("Current process is not null \n");*/
         /*if the process is still around need to copy its contents over*/
        CtrlPlusC(temp, &(currentProcess->p_s));
        insertProcQ(&readyQue, currentProcess);
        /*Load the state back */
        /**LDST(temp);*/
        fuckmylife(100);
           /*addokbuf("Calling scheduler \n");*/
       scheduler();
    
        
       

    }
    else{
           addokbuf("Current Proc null \n");
   /* LDST(currentProcess);*/
      scheduler();
    }
}


int fuckmylife(int asd){


    return asd; 
}
