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

/* Global Variables*/
int processCount;
int softBlockCount;
pcb_t* currentProcess;
pcb_t* readyQue;



/*LET THE OS KNOW WHERE TO START!*/
int main(){

    /*  populate the four new areas in low memory
            -set the stack pointer(last page of physical)
            -set PC
            -set the status:    - VM off
                                - Interrupts masked
                                - Supervisor mode on
    */



    /*  Initialize the PCB and ASL lists  */
    initPcbs();
    initASL();


    /*  Initialize phase2 global variables  */
    processCount = 0;
    softBlockCount = 0;
    currentProcess = NULL;
    readyQue = mkEmptyProcQ();




    /*  p = allocatePCB()
            -initialize the p_s
            -stack pointer to the penultimate page of physical memory
            -PC(p2test)
            -status:    -VM off
                        -Interupts Enabled
                        -Supervisor Mode on
            
    */





    processCount++;              /* Adds one more process to the process count */
    insertProQ(&readyQue, p);    /* Inserts the proces into the pcb data structure */
    
    currentProcess = NULL;
    
    /* Lets the scheduler file take over.*/
    scheduler();
        
    return 0;
    



}