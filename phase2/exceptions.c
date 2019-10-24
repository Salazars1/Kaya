/*  PHASE 2
    Written by NICK STONE AND SANTIAGO SALAZAR
    Base code and Comments from PROFESSOR MIKEY G 
    Finished on 
*/

/*********************************************************************************************
                            Module Comment Section
    A SYSCALL exception occurs when a SYSCALL assembler instruction is executed. After
    the processor and ROM-Exception handler actions when a SYSCALL point exception is 
    raised, continued executing the nucleus Exception handler. This nreakpoints, are 
    distinguidhed from a Breakpoint, exceptions by the contents of Cause.ExcCode in the
    SYSOldArea. There are 8 Sys calls, which are going to be defined with more detail in 
    each function documentation.
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
extern cpu_t currentTOD;
extern cpu_t TODStart;

/*  Declaration of exceptions and helper fucntions. Further documentation will be provided
    in the actual function.*/
HIDDEN void Syscall1(state_t *caller);
HIDDEN void Syscall2();
HIDDEN void Syscall3(state_t *caller);
HIDDEN void Syscall4(state_t *caller);
HIDDEN void Syscall5(state_t *caller);
HIDDEN void Syscall6(state_t *caller);
HIDDEN void Syscall7(state_t *caller);
HIDDEN void Syscall8(state_t *caller);

HIDDEN void PassUpOrDie(state_t *caller, int triggerReason);
 void PrgTrapHandler();
void TLBTrapHandler();

extern void CtrlPlusC(state_t *oldState, state_t *newState);
HIDDEN void NukeThemTillTheyPuke(pcb_t *headPtr);



/*  There are 8 System calls (Syscall 1 through Syscall 8) that our Handler must look out
    for these first 8 System calls the Kernel Mode must be active in order for these commands
    to execute. If this is not the case, then the appropiate program trap would be execute. 
    Parameters: None
    Return: Void
     */

    int testb(int a){
        return a;
    }
void SYSCALLHandler()
{
    /*addokbuf("An Exception has happened we are in the SYscall handler\n");*/
    state_t *prevState;
    state_t *program;
    unsigned int prevStatus;

    int casel;
    int mode;
    

    prevState = (state_t *)SYSCALLOLDAREA; /* prevState status*/
    prevStatus = prevState->s_status;
    casel = prevState->s_a0;
    /*addokbuf("Exceptions have been loaded check fuck me test for casel sys call val\n");*/
 /*   testb(casel);*/
    mode = (prevStatus & UMOFF); /*Uses the compliment to determine the mode I'm in*/

    if (((prevStatus > 0) && (prevStatus < 9) && mode) != ALLOFF)
    { /* It is User Mode*/
        /*addokbuf("We are in the part where the program will die\n");*/    
        program = (state_t *)PRGMTRAPOLDAREA;
        CtrlPlusC(prevState, program);

        /*setting Cause.ExcCode in the Program Trap Old Area to Reserved Instruction */
        (program->s_cause) = (((program->s_cause) & ~(0xFF)) | (10 << 2));
        /*addokbuf("Program will be killed\n");*/
        /*Program Trap Handler */
        PrgTrapHandler();
    }

    /* increment prevState's PC to next instruction */
    (prevState->s_pc) = (prevState->s_pc) + 4;
    /*addokbuf("Get the next instruction\n");*/
    /*Switch statement to determine which Syscall we are about to do. If there is no case, we
    execute the default case */
    switch (casel)
    {

    case SYSCALL1:
        
        Syscall1(prevState);
        break;

    case SYSCALL2:
        Syscall2();
        break;

    case SYSCALL3:
        addokbuf("Sys call 6 \n");

        Syscall3(prevState);
        break;

    case SYSCALL4:
        addokbuf("Sys call 6 \n");

        Syscall4(prevState);
        break;

    case SYSCALL5:
        Syscall5(prevState);
        break;

    case SYSCALL6:
    addokbuf("Sys call 6 \n");
        Syscall6(prevState);
        break;

    case SYSCALL7:
        Syscall7(prevState);
        break;

    case SYSCALL8:
        Syscall8(prevState);
        break;

    default:
    /*addokbuf("Pass up or die (Default case\n");*/
        PassUpOrDie(prevState, SYSTRAP);
        break;
    }

    /*We should NEVER GET HERE. IF WE DO, WE DIE*/
    /*addokbuf("Panic\n");*/
    PANIC();
}

/**************************  SYSCALL 1 THROUGH 8 FUNCTIONS    ******************************/

/*  This service creates a new process (progeny) of the caller process. a1 contains the physical
    address of a processor state area at the rime instruction is executed. The processsor state
    is uded as the initial state of the newly birth child.
    Parameter:  state* caller
    Return:     -0 in V0 if the process was done effectively
                -1 in V0 if the process was NOT done because of lack of resources.*/

HIDDEN void Syscall1(state_t *caller)
{
    /*addokbuf("calling Alloc PCB\n");*/
    pcb_t *birthedProc = allocPcb();

    if (emptyProcQ(birthedProc))
    { /*Check space in the ready queue to make sure we have room to allocate*/
        /*We did not have any more processses able to be made so we send back a -1*/
        /*addokbuf("No More processes left load state\n");*/
        caller->s_v0 = -1;
    }
    else
    {
        /*addokbuf("Process count gets incremented\n");*/
        processCount++;

        /*Makes the new process a child of the currently running process calling the sys call */
        insertChild(currentProcess, birthedProc);

        /* Inserts the new process into the Ready Queue*/
        insertProcQ(&readyQue, birthedProc);

        /*Copy the calling state into the new processes state*/
        CtrlPlusC(((state_t *)caller->s_a1), &(birthedProc->p_s));
        /*addokbuf("INserted into the process and child Copy state\n");*/
        /*WE were able to allocate thus we put 0 in the v0 register*/
        caller->s_v0 = 0;
        /*addokbuf("Load state and we done\n");*/
    }
    LDST(caller);
}

/*  This services causes the executing process to be anihilated along with all its children, grand
    children and so on. Execution of this instruction does not complete until everyone has been
    exterminated
    Parameters: None
    Return: Void*/
HIDDEN void Syscall2()
{

    NukeThemTillTheyPuke(currentProcess);
    /*call scheduler*/
    /*addokbuf("Schedule is called\n");*/
    scheduler();
}

/*  When this service is requested, it is interpreted by the nucleus to request to perform a Verhogen
    (V) operation on a sempahore. This is requested by placing 3 in a0, abd Verhogened in a1.
    Parameter:  state* caller
    Return: Void
    */
HIDDEN void Syscall3(state_t *caller)
{
    /*addokbuf("SYSCALL 3 \n");*/
    /*addokbuf("Creating a new process\n");*/
    pcb_t* newProccess = NULL;
    /*addokbuf("Get the semaphore Callers A1\n");*/
    int * sema = (int *)caller ->s_a1; 
    ++(*sema);
     /* increment semaphore  */
   /* testb(caller -> s_a1);*/
    if (*sema <= 0)
    { /* waiting in the semaphore */

        /*addokbuf("Caller A1 is less than or equal to 0\n");*/
        newProccess = removeBlocked(sema);
        if (newProccess != NULL)
        { /* add it to the ready queue */
            newProccess ->p_semAdd = NULL;
            /*addokbuf("Newprocess is not null put that on the ready queue\n");*/
            insertProcQ(&readyQue, newProccess);
        }
    }
    
    /*addokbuf("Load State\n");*/
    LDST(caller); /* returns control to caller */
}

/*  When this service is requested, it is interpreted by the nucleus to request to perform a Passeren
    (P) operation on a sempahore. This is requested by placing 4 in a0, and Passerened in a1.
    Parameter:  state* caller
    Return: Void
    */
HIDDEN void Syscall4(state_t *caller)
{

    /*addokbuf("Syscall 4 start\n ");*/
    int * sema = (int * ) caller->s_a1; /* decrement semaphore */
  /*  testb(caller -> s_a1);*/

  (*sema)--;
    if (*sema < 0)
    { /* there is something controlling the semaphore */
        CtrlPlusC(caller, &(currentProcess->p_s));
        insertBlocked(sema, currentProcess);
       /*addokbuf("the sa1 is less than 0 COpy state and block the process\n");*/
        scheduler();

    }
    /* nothing had control of the sem, return control to caller */
    /*addokbuf("Sys call 4 load state\n");*/
    LDST(caller);
}

/*  When this service is requested, it will save the contentes of a2 and a3 and pass them to handle the
    respective exceptions (TLB, PGMTRAP, SYS) while this process is executing. Each process may request
    a SYS5 only ONCE for each of the exceptions types, more than one call will trigger SYS2 and Nuke the
    process (error occured).
    Parameter:  state* caller
    Return: Void
    */
HIDDEN void Syscall5(state_t *caller)
{   
    /*addokbuf("Syscall 5 start\n");*/

    if (caller->s_a1 == 0)
    { /*TLB TRAP*/
    /*addokbuf("the calling state a1 is 0\n");*/
        if (currentProcess->p_newTLB != NULL)
        { /* already called sys5 */
            /*addokbuf("Syscall 2 is called\n");*/
            Syscall2();
        }
        /* assign exception values */
        currentProcess->p_oldTLB = (state_t *)caller->s_a2;
        currentProcess->p_newTLB = (state_t *)caller->s_a3;
        /*addokbuf("Current process TLB process is being set\n");*/
    }
    else if (caller->s_a1 == 1)
    { /*Program Trap*/
    /*addokbuf("the calling state is 1 so new program trap \n");*/
        if ((currentProcess->p_newProgramTrap) != NULL)
        { /* already called sys5 */
            /*addokbuf("Syscall2\n ");*/
            Syscall2();
        }
        /* assign exception values */
        currentProcess->p_oldProgramTrap = (state_t *)caller->s_a2;
        currentProcess->p_newProgramTrap = (state_t *)caller->s_a3;
        /*addokbuf("Current Process new program or old program is set to a2 and a3\n");*/
    }
    else
    {

        if ((currentProcess->p_newSys) != NULL)
        { /* already called sys5 */
            /*addokbuf("Calling sys call 2\n");*/
            Syscall2();
        }
        /* assign exception values */
        /*addokbuf("Old sys and new sys is set to a2 and a3\n");*/
        currentProcess->p_oldSys = (state_t *)caller->s_a2;
        currentProcess->p_newSys = (state_t *)caller->s_a3;
    }
/*addokbuf("Load State\n");*/
    LDST(caller);
}

/*Syscall6:  "Get_CPU_Time"
    This service is in charge of making sure that the amount of time spent being processed is tracked by 
    each Process Block that is running. 
        Parameters: State_t * caller
        Return: Void*/
HIDDEN void Syscall6(state_t *caller)
{
    /*addokbuf("Sys call 6 start\n");*/
   CtrlPlusC(caller, &(currentProcess->p_s));
    
    STCK(currentTOD);
    

    /*Track the amout of time spent processing and add this to the previous amount of process time*/
    /*addokbuf("Time is being set properly\n");*/
    (currentProcess->p_timeProc) = (currentProcess->p_timeProc) + (currentTOD - TODStart);
    /*Store the new updated time spent processing into the v0 register of the process state*/
    (currentProcess->p_s.s_v0) = (currentProcess->p_timeProc);

    /*Updates start time*/

    STCK(TODStart);
    /*Load the Current Processes State*/
   /*addokbuf("Load State\n");*/
    LDST(&(currentProcess->p_s));
}

/*  Syscall 7 performs a syscall 4 on the Semaphore associated to clock timer
    Knowing that this clock also has a syscall 3 performing on it every 100 milliseconds
    Parameters: State_t* Caller
    Return: Void*/
HIDDEN void Syscall7(state_t *caller)
{
    /*addokbuf("Syscall 7 start\n");*/
    int *sem;
    sem = (int *)&(semD[SEMNUM - 1]);
    (*sem)--;
  /*  testb(*sem);*/
    if (*sem < 0)
    {
        /*addokbuf("Semaphore is less than 0\n");*/
        /*Sem is less than 0 block the current process*/

        CtrlPlusC(caller, &(currentProcess->p_s));
        insertBlocked(sem, currentProcess);
     
        /*Increment that we have another process soft block so that it does not starve*/
        softBlockCount++;
    }


    /*Process is soft blocked call to run another process*/
    /*addokbuf("Call Scheduler\n");*/
    scheduler();
}

/*  This service perofroms a Syscall 5 operation on the semaphore that the nucles maintains for the IO 
    device by the values in a1, a2, and a3 (optionally). The nucleus performs a V operation on the
    semaphore whenever that device generates an interrupt. 
    Return:     Device Status in v0 (Once the process resumes after the occurrence of the anticipated
                interrupt)*/
HIDDEN void Syscall8(state_t *caller)
{
    /*addokbuf("Syscall 8 \n");*/

    int lineNo; /*  line number*/
    int dnum;   /*  device number*/
    int termRead;
    int index;
    int *sem;

    lineNo = (int)caller->s_a1;
    dnum = (int)caller->s_a2;
    termRead = (int)caller->s_a3; /* terminal read  or write */
/*testb(lineNo);
testb(dnum);
testb(termRead);*/
    /* what device is going to be computed*/
    /*addokbuf("Store values from registers a1 a2 a3 \n");*/
    index = lineNo -3 + termRead; 
    index = index * 8; 
    index = index + dnum; 

    sem = &(semD[index]);
/*addokbuf("We are messing with semaphores again\n");*/
   /* test(*sem);*/

   (*sem)--;
    if (*sem < 0)
    {

        /*addokbuf("Copying state and inserting it onto the blocked list\n");*/
        CtrlPlusC(caller, &(currentProcess->p_s));
        insertBlocked(sem, currentProcess);
       

        softBlockCount++; 

        /*DECIDED TO CALL SCHEDULER instead of giving back time to the process that was interrupted
        Keeps the overall flow of the program and since there is no starvation, eventually that process
        will get its turn to play with the processor*/
        /*LDST(caller);*/
       
        /*addokbuf("Calling scheduler\n");*/
        scheduler();
    }

    
}

/**************************  HANDLERS FUNCTIONS    ******************************/

/*If an exception has been encountered, it passes the error to the appropiate handler, if no exception
    is found, it Nukes the procees till it pukes.
    Parameters: state_t *caller
    Return: Void 
    */
void PassUpOrDie(state_t *caller, int triggerReason)
{
   
    state_t *oldState;
    state_t *newState;

    switch (triggerReason)
    {

    case TLBTRAP: /*0 is TLB EXCEPTIONS!*/
    /*addokbuf("TLB Trap \n");*/
        if ((currentProcess->p_newTLB) != NULL)
        {
            oldState = currentProcess->p_oldTLB;
            newState = currentProcess->p_newTLB;
        }
        else
        {
            Syscall2();
        }
        break;

    case PROGTRAP: /*1 is Program Trap Exceptions*/
    /*addokbuf("Program trap \n");*/
        if ((currentProcess->p_newProgramTrap) != NULL)
        {
            oldState = currentProcess->p_oldProgramTrap;
            newState = currentProcess->p_newProgramTrap;
        }
        else
        {
            Syscall2();
        }
        break;

    case SYSTRAP: /*2 is SYS Exception!*/
    /*addokbuf("Sys trap");*/
        if ((currentProcess->p_newSys) != NULL)
        {
            oldState = currentProcess->p_oldSys;
            newState = currentProcess->p_newSys;
        }
        else
        {
            Syscall2();
        }
        break;

    default:
        Syscall2(); /*No vector is defined. Nuke it till it pukes*/
        break;
    }

    CtrlPlusC(oldState, newState);
    LDST(newState);
}

/*Gets triggered when the executing process performs an illegal operation. Therefore, since  this is 
    triggered when a PgmTrap exception is raised, execution continues with the nucleus’s PgmTrap exception
    handler. The cause of the PgmTrap exception will be set in Cause.ExcCode in the PgmTrap Old Area.
    Parameters: None
    Return: Void
     */
void PrgTrapHandler()
{
    /*addokbuf("Progrma trap handler is being called\n");*/
    state_t *caller = (state_t *)PRGMTRAPOLDAREA;
    /*Call Pass Up Or Die*/
    PassUpOrDie(caller, PROGTRAP);
}

/*Gets triggered when μMPS2 fails in an attempt to translate a virtual address into its corresponding 
    physical address. Therefore, since  this is triggered when a TLB exception is raised, execution
    continues with the nucleus’s TLB exception handler. The cause of the TLB exception will be set in
     Cause.ExcCode in the TLB Old Area. 
     Parameters: None
    Return: Void
     */
void TLBTrapHandler()
{
    /*addokbuf("TLB Program trap is being called\n");*/
    state_t *caller = (state_t *)TLBMGMTOLDAREA;
    /*Call Pass Up Or Die*/
    PassUpOrDie(caller, TLBTRAP);
}

/**************************  HELPER FUNCTIONS    ******************************/

/*Recursively removes all the children of the head, and starts hunting them down one by one. 
    It kills them if they are in the ASL, ReadyQueue or currentProcess. Adjust the process count
    as the process are being terminated.
    Parameters: pcb_t * HeadPtr
    Return: Void
    */
HIDDEN void NukeThemTillTheyPuke(pcb_t *headPtr)
{
    addokbuf("Mother fucjer");
    while (!emptyChild(headPtr))
    {
        addokbuf("REEEE");
        /*We are going to the bottom most child to KILL every child in list (Rinse and Repeat)*/
        NukeThemTillTheyPuke(removeChild(headPtr));
    }

    if (headPtr == currentProcess)
    {
        addokbuf("COOKING");
        /*  Children services comes for you and take your child*/
        outChild(headPtr);
    }
    addokbuf("Here\n");
    if (headPtr->p_semAdd != NULL)
    {
        /*  remove process from readyQueue*/
        int *sema4 = (headPtr->p_semAdd);
        /*  remove process from ASL*/
        outBlocked(headPtr);
        if (sema4 >= &(semD[0]) && sema4 <= &(semD[SEMNUM - 1]))
        { /*SemAdd count is somewhere in between the SemD array*/
            softBlockCount--;
        }
        else
        {
            /*  Increment Semaphore*/
            (*sema4)++;
        }
    }
    else
    {
       


         outProcQ(&readyQue, headPtr);
    }
    /*  We have no more children! Good to go*/
    freePcb(headPtr);
    processCount--;
}

/*  This state will copy all of the contents of the old state into the new state
    Parameters: State_t * oldstate, State_t* NewState
    Return: Void*/
extern void CtrlPlusC(state_t *oldState, state_t *newState)
{
    /*Move all of the contents from the old state into the new*/
    newState->s_asid = oldState->s_asid;
    newState->s_status = oldState->s_status;
    newState->s_pc = oldState->s_pc;
    newState->s_cause = oldState->s_cause;
    /*Loop through all of the registers in the old state and write them into the new state*/
    int i;
    for (i = 0; i < STATEREGNUM; i++)
    {
        newState->s_reg[i] = oldState->s_reg[i];
    }
}
