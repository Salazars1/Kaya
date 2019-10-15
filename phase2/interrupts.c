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
    int devsemnum;
    int devicenumber;
    int deviceRegisterNumber;
    int semaphoreaddress; 
    unsigned int Devicestatus; 
    cpu_t timeInterruptOccurs;
    devregarea_t *OffendingDevice;
    
    state_PTR caller;
    caller = (state_t *)INTERRUPTOLDAREA;
    

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
    devregarea_t * temporary = (devregarea_t *)DEVPHYS;




    if(devicenumber == -1){
        PANIC();
    }


  


/*Need to Determine Device Address and the Device semaphore number*/
int templinenum; 
/*Offest the Line number*/
templinenum = lineNumber - 3; 

/* 8 devices per line number*/
devsemnum = lineNumber * 8; 
/*We know which device it is */
devsemnum = devsemnum + devicenumber;

/*The base + 32 (4 words in the device + the size of each register * the register number*/
deviceRegisterNumber = (device_t *)temporary ->rambase +32 + (devsemnum * DEVREGSIZE);


  if(lineNumber == TERMINT){

        if(deviceRegisterNumber -> t_transm_status &0x0F) != READY){
            /*Acknowledge*/
           Devicestatus = deviceRegisterNumber ->t_transm_status;
           /*Acknowledge*/
           deviceRegisterNumber -> t_transm_command = ACK;

        }
        else{
            /*Save the status*/
            Devicestatus = deviceRegisterNumber ->t_recv_status; 
            /*Acknowledge*/
            deviceRegisterNumber ->t_recv_command = ACK;
            /*fix the semaphore number for terminal readers sub device */
            devsemnum = devsemnum + DEVPERINT;


         }       


    }
    else{
        /*Non terminal Interrupt*/
        Devicestatus = deviceRegisterNumber ->d_status;
        /*Acknowledge the interrupt*/ 
        deviceRegisterNumber ->d_command = ACK;


    }

/*V op */
    semaphoreaddress = semD[devsemnum];
    (*semaphoreaddress)++;
    if(semaphoreaddress <= 0){

        t = removeBlocked(semaphoreaddress);
        if(t != NULL){
            t->p_s.s_v0 = temporary ->devreg ->d_status; 
            softBlockCount--;
            insertProcQ(&readyQue, t); 
        }


    }
    /*Interrupt has been Handled!*/
    CallScheduler();
}


/*HELPER FUNCTIONS*/

int finddevice(int linenumber)
{
    /*Set some local variables*/
    int i;
    OffendingDevice = (devregarea_t *)DEVPHYS;
    /*make a copy of the bit map */
    unsigned int map = OffendingDevice->interrupt_dev[linenumber];
    int devn;
    /*8 Total devices to look through */
    for (i = 0; i < TOTALDEVICES; i++)
    {
        /*Bit wise and if the value is not 0 Device is interrupting */
        if ((map & FIRSTBIT) != ZERO)
        {

            devn = i;
        }
        {
        else
            /*Increment both the index and shift the bits 1 */
            i = i + 1;
            map << 1;
        }
    }
/*Return the device number*/
    return devn;
}



HIDDEN void CallScheduler()
{
    state_t * temp = (state_t *) INTERRUPTOLDAREA
    if (currentProcess == NULL)
    {
        /*Get the next Job */
        scheduler();
    }
    else
    {
       /*if the process is still around need to copy its contents over*/
    
     
       CtrlPlusC(temp, currentProcess ->p_s);
       /*Load the state back */
       LDST(temp);
    }
}