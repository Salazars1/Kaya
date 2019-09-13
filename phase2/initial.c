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
            -stack pointer to the penultimate page of ohysical memory
            -PC(p2test)
            -status:    -VM off
                        -Interupts Enabled
                        -Supervisor Mode on
    */
    


    
    return 0;
    



}