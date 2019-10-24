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
cpu_t interruptstart; 

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
    STCK(interruptstart);
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
      
        PANIC();
       
    }
    else if ((offendingLine & CLOCK1) != ZERO)
    {
         /*  addokbuf("QUantum is up CLock 1 call scheduler \n");*/
        /*The process has spent its quantum. Its time to start a new process .*/
    
        CallScheduler();
        /*Clock 1 Has an Interrupt */
    }
   
    else if ((offendingLine & CLOCK2) != ZERO)
    {
        /*Load the clock with 100 Milliseconds*/
          /*addokbuf("Psuedo Clock\n");*/
        LDIT(PSUEDOCLOCKTIME);
        /*Access the Last clock which is the psuedo clock*/
        semaphoreAddress = (int *) &(semD[SEMNUM-1]);
        testingbaby(5);
        cpu_t finish; 
        STCK(finish);
       
        while(headBlocked(semaphoreAddress) != NULL)
        {
               /*addokbuf("Headblocked is running \n");*/
            
            t = removeBlocked(semaphoreAddress);
               /*addokbuf("Remove the process from the blocked \n");*/

            if(t != NULL){
                   /*addokbuf("new process is not null \n");*/
                insertProcQ(&readyQue, t);
                t -> p_timeProc = t -> p_timeProc + (finish - interruptstart);
                softBlockCount--;
            }
        }
         
        (*semaphoreAddress) = 0;
      /* addokbuf("Reset the semaphore address and call scheduler \n");*/
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
          /* addokbuf("the offending line is a terminal device -> FUck  \n");*/
        /*Terminal Device is on */
         /*addokbuf("FUCK \n\n\n\n\n");*/
        lineNumber = TERMINALI;
    }
    else
    {
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

        PANIC();
    }
   
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
/*addokbuf("WE LIVE \n\n");*/
    if (lineNumber == TERMINT)
    {

           /*addokbuf("We got a terminal \n");*/
        /*Terminal*/

        if ((testing->t_transm_status & 0x0F) != READY)
        {
                   /*addokbuf("We are transmitting \n");*/
                /*Acknowledge*/
            
                deviceStatus = devaddrbase->t_transm_status;
         
                /*Acknowledge*/
                devaddrbase->t_transm_command = ACK;

       
        }
        else
        {
               addokbuf("We are recieving  \n\n\n\n\n\n\n\n");
            /*Save the status*/
            devsemnum += 8; 
          
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


    /*V op */
       /*addokbuf("Playing with semaphores go ahead and check these in the debug functions \n");*/
    int * semad; 

    semad =&(semD[devsemnum]);

    
    (*semad)--; 
   /* tes(*semad);*/
    
    
    if ((*semad) <= 0)
    {
        /*addokbuf("Value is less than 0 \n");*/
        t = removeBlocked(semad);
        if (t != NULL)
        {
               /*addokbuf("t is a process that was on the blocked queue \n");*/
            t->p_semAdd = NULL;
            t-> p_s.s_v0 = deviceStatus; 
             softBlockCount--;
            insertProcQ(&readyQue, t);
           
            
        }
       
    }
    
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
    
    /*Return the device number*/
    return devn;
}

HIDDEN void CallScheduler()
{
       /*addokbuf("Calling the shceduler has started \n");*/
    state_t *temp;
    temp =  (state_t *)INTERRUPTOLDAREA;
    cpu_t finished; 
    
    if (currentProcess != NULL)
    {
           /*addokbuf("Current process is not null \n");*/
         /*if the process is still around need to copy its contents over*/
         STCK(finished);
         TODStart = finished + interruptstart;
        CtrlPlusC(temp, &(currentProcess->p_s));
        insertProcQ(&readyQue, currentProcess);
        /*Load the state back */
        /**LDST(temp);*/
    
           /*addokbuf("Calling scheduler \n");*/
       scheduler();
    
        
       

    }
    else{
           /*addokbuf("Current Proc null \n");*/
   /* LDST(currentProcess);*/
      scheduler();
    }
}


