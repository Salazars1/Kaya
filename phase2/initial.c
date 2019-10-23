/*  PHASE 2
    Written by NICK STONE AND SANTIAGO SALAZAR
    Base comments and some assitance from PROFESSOR MIKEY G 
    Finished on  
*/

/*********************************************************************************************
                            Module Comment Section
    Every program needs a main. This module is the entry point of our Kaya Implementation 
    Operative System. This module populates new areas in the ROM Reserved Frame, initializes
    phase 1 code, as well as nucleus maintained variables, the nucleus maintained semaphores,
    instantes a single process in the Ready Queue, and calls the scheduler.
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


#define TRANSMITTED	5
#define ACK	1
#define PRINTCHR	2
#define CHAROFFSET	8
#define STATUSMASK	0xFF
#define	TERM0ADDR	0x10000250


/* Global Variables*/
int processCount;
int softBlockCount;
pcb_t *currentProcess;
pcb_t *readyQue;
int semD[SEMNUM];
char okbuf[2048];			/* sequence of progress messages */
char *mp = okbuf;
typedef unsigned int devreg;

extern void test(); 
extern void addkbuf(char * strp);
/*LET THE OS KNOW WHERE TO START!*/
int main()
{
    devregarea_t* deviceBus;
    deviceBus = (devregarea_t*) RAMBASEADDR;
    unsigned int RAMTOP;                                         /* Defines RAMTOP as an unsigned integer*/
    RAMTOP = (deviceBus->rambase) + (deviceBus->ramsize);   /*Sets RAMTOP according to the hardware memory*/

    state_t* newLocation; /* Initialize the new Processor State Areas */

        /*  Initialize the PCB and ASL lists  */
    initPcbs();
    initASL();


    /*  Initialize phase2 global variables  */
    processCount = 0;
    softBlockCount = 0;
    currentProcess = NULL;
    readyQue = mkEmptyProcQ();

    /* iniltialize semaphores to 0*/
    int i;
    for (i = 0; i < SEMNUM; i++)
    {
        semD[i] = 0;
    }

    /* SYSCALL BREAK*/
    newLocation = (state_t*) SYSCALLNEWAREA;
    newLocation->s_pc = (memaddr) SYSCALLHandler;     
    newLocation->s_t9 = (memaddr) SYSCALLHandler;
    newLocation->s_sp = RAMTOP;
    newLocation->s_status = ALLOFF;  /* Turns the VMOFF, IMON, UMOFF (Checks const.h for info in the names) */

    /* PROGRAM TRAP */
    newLocation = (state_t*) PRGMTRAPNEWAREA;
    newLocation->s_pc = (memaddr) PrgTrapHandler;
    newLocation->s_t9 = (memaddr) PrgTrapHandler;
    newLocation->s_sp = RAMTOP;
    newLocation->s_status = ALLOFF; /* Turns the VMOFF, IMON, UMOFF (Checks const.h for info in the names) */

    /* TLB MANAGEMENT */
    newLocation = (state_t*) TLBMGMTNEWAREA;
    newLocation->s_pc = (memaddr) TLBTrapHandler;
    newLocation->s_t9 = (memaddr) TLBTrapHandler;
    newLocation->s_sp = RAMTOP;
    newLocation->s_status = ALLOFF; /* Turns the VMOFF, IMON, UMOFF (Checks const.h for info in the names) */

    /* INTERRUPTS */
    newLocation = (state_t*) INTERRUPTNEWAREA;
    newLocation->s_pc = (memaddr) IOTrapHandler;
    newLocation->s_t9 = (memaddr) IOTrapHandler;
    newLocation->s_sp = RAMTOP;
    newLocation->s_status = ALLOFF; /* Turns the VMOFF, IMON, UMOFF (Checks const.h for info in the names) */

    /* Create initial process (alloc PCB)*/
    currentProcess = allocPcb();
    processCount = processCount + 1;           /* Adds one more process to the process count */

    /* Initialize p_s with all the requirements */
    currentProcess->p_s.s_sp = (RAMTOP - PAGESIZE);
    currentProcess->p_s.s_pc = (memaddr) test;
    currentProcess->p_s.s_t9 = (memaddr) test;
    currentProcess->p_s.s_status = ALLOFF | IEON | IMON | TEBITON; /* Turns the VMOFF, IMON, UMOFF (Checks const.h for info in the names) */

    
    insertProcQ(&readyQue, currentProcess); /* Inserts the process into the pcb data structure */
    currentProcess = NULL;
    
    LDIT(IOCLOCK);  /*Sets the semaphore pseudoclock*/

    /* Lets the scheduler file take over.*/
    scheduler();

    return 0;
}




devreg termstat(memaddr * stataddr) {
	return((*stataddr) & STATUSMASK);
}

/* This function prints a string on specified terminal and returns TRUE if 
 * print was successful, FALSE if not   */
unsigned int termprint(char * str, unsigned int term) {
	memaddr * statusp;
	memaddr * commandp;
	devreg stat;
	devreg cmd;
	unsigned int error = FALSE;
	
	if (term < DEVPERINT) {
		/* terminal is correct */
		/* compute device register field addresses */
		statusp = (devreg *) (TERM0ADDR + (term * DEVREGSIZE) + (TRANSTATUS * DEVREGLEN));
		commandp = (devreg *) (TERM0ADDR + (term * DEVREGSIZE) + (TRANCOMMAND * DEVREGLEN));
		
		/* test device status */
		stat = termstat(statusp);
		if (stat == READY || stat == TRANSMITTED) {
			/* device is available */
			
			/* print cycle */
			while (*str != EOS && !error) {
				cmd = (*str << CHAROFFSET) | PRINTCHR;
				*commandp = cmd;

				/* busy waiting */
				stat = termstat(statusp);
				while (stat == BUSY)
					 stat = termstat(statusp);
				
				/* end of wait */
				if (stat != TRANSMITTED)
					error = TRUE;
				else
					/* move to next char */
					str++;
			} 
		}
		else
			/* device is not available */
			error = TRUE;
	}
	else
		/* wrong terminal device number */
		error = TRUE;

	return (!error);		
}


/* This function placess the specified character string in okbuf and
*	causes the string to be written out to terminal0 */
void addokbuf(char *strp) {
	char *tstrp = strp;
	while ((*mp++ = *strp++) != '\0')
		;
	mp--;
	termprint(tstrp, 0);
}

