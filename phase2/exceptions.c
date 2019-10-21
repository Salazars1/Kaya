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

void PassUpOrDie(state_t *caller, int triggerReason);
void PrgTrapHandler();
void TLBTrapHandler();

extern void CtrlPlusC(state_t *oldState, state_t *newState);
HIDDEN void NukeThemTillTheyPuke(pcb_t *headPtr);
HIDDEN void LoadState(state_t *s);

int fuckme(int b)
{
    return b;
}
/*  There are 8 System calls (Syscall 1 through Syscall 8) that our Handler must look out
    for these first 8 System calls the Kernel Mode must be active in order for these commands
    to execute. If this is not the case, then the appropiate program trap would be execute. 
    Parameters: None
    Return: Void
     */
void SYSCALLHandler()
{
    state_t *prevState;
    state_t *program;
    unsigned int prevStatus;

    int casel;
    int mode;

    prevState = (state_t *)SYSCALLOLDAREA; /* prevState status*/
    prevStatus = prevState->s_status;
    casel = prevState->s_a0;

    mode = (prevStatus & UMOFF); /*Uses the compliment to determine the mode I'm in*/

    if (((prevStatus > 0) && (prevStatus < 9) && mode) != ALLOFF)
    { /* It is User Mode*/
        program = (state_t *)PRGMTRAPOLDAREA;
        CtrlPlusC(prevState, program);

        /*setting Cause.ExcCode in the Program Trap Old Area to Reserved Instruction */
        (program->s_cause) = (((program->s_cause) & ~(0xFF)) | (10 << 2));

        /*Program Trap Handler */
        PrgTrapHandler();
    }

    /* increment prevState's PC to next instruction */
    (prevState->s_pc) = (prevState->s_pc) + 4;

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
        Syscall3(prevState);
        break;

    case SYSCALL4:
        Syscall4(prevState);
        break;

    case SYSCALL5:
        Syscall5(prevState);
        break;

    case SYSCALL6:
        Syscall6(prevState);
        break;

    case SYSCALL7:
        Syscall7(prevState);
        break;

    case SYSCALL8:
        Syscall8(prevState);
        break;

    default:
        PassUpOrDie(prevState, SYSTRAP);
        break;
    }

    /*We should NEVER GET HERE. IF WE DO, WE DIE*/
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

    pcb_t *birthedProc = allocPcb();

    if (birthedProc == NULL)
    { /*Check space in the ready queue to make sure we have room to allocate*/
        /*We did not have any more processses able to be made so we send back a -1*/
        caller->s_v0 = -1;
        LDST(caller);
    }
    else
    {
        processCount++;

        /*Makes the new process a child of the currently running process calling the sys call */
        insertChild(currentProcess, birthedProc);

        /* Inserts the new process into the Ready Queue*/
        insertProcQ(&readyQue, birthedProc);

        /*Copy the calling state into the new processes state*/
        CtrlPlusC(((state_t *)caller->s_a1), &(birthedProc->p_s));

        /*WE were able to allocate thus we put 0 in the v0 register*/
        caller->s_v0 = 0;

        LoadState(caller);
    }
}

/*  This services causes the executing process to be anihilated along with all its children, grand
    children and so on. Execution of this instruction does not complete until everyone has been
    exterminated
    Parameters: None
    Return: Void*/
HIDDEN void Syscall2()
{
    if (emptyChild(currentProcess))
    { /*current process has no children*/
        outChild(currentProcess);
        freePcb(currentProcess);
        processCount--;
    }
    else
    {
        /*Helper Function*/
        NukeThemTillTheyPuke(currentProcess);
    }

    /*  Clean up after myself*/
    currentProcess = NULL;

    /*call scheduler*/
    scheduler();
}

/*  When this service is requested, it is interpreted by the nucleus to request to perform a Verhogen
    (V) operation on a sempahore. This is requested by placing 3 in a0, abd Verhogened in a1.
    Parameter:  state* caller
    Return: Void
    */
HIDDEN void Syscall3(state_t *caller)
{
    pcb_t* newProccess = NULL;
    (caller->s_a1)++; /* increment semaphore  */

    if ((caller->s_a1) <= 0)
    { /* waiting in the semaphore */
        newProccess = removeBlocked((caller->s_a1));
        if (newProccess != NULL)
        { /* add it to the ready queue */
            insertProcQ(&readyQue, newProccess);
        }
    }

    LoadState(caller); /* returns control to caller */
}

/*  When this service is requested, it is interpreted by the nucleus to request to perform a Passeren
    (P) operation on a sempahore. This is requested by placing 4 in a0, and Passerened in a1.
    Parameter:  state* caller
    Return: Void
    */
HIDDEN void Syscall4(state_t *caller)
{
    (caller->s_a1)--; /* decrement semaphore */
    if ((caller->s_a1) < 0)
    { /* there is something controlling the semaphore */
        CtrlPlusC(caller, &(currentProcess->p_s));
        insertBlocked((caller->s_a1), currentProcess);
        scheduler();
    }
    /* nothing had control of the sem, return control to caller */
    LoadState(caller);
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

    if (caller->s_a1 == 0)
    { /*TLB TRAP*/
        if (currentProcess->p_newTLB != NULL)
        { /* already called sys5 */
            Syscall2();
        }
        /* assign exception values */
        currentProcess->p_oldTLB = (state_t *)caller->s_a2;
        currentProcess->p_newTLB = (state_t *)caller->s_a3;
    }
    else if (caller->s_a1 == 1)
    { /*Program Trap*/
        if ((currentProcess->p_newProgramTrap) != NULL)
        { /* already called sys5 */
            Syscall2();
        }
        /* assign exception values */
        currentProcess->p_oldProgramTrap = (state_t *)caller->s_a2;
        currentProcess->p_newProgramTrap = (state_t *)caller->s_a3;
    }
    else
    {
        if ((currentProcess->p_newSys) != NULL)
        { /* already called sys5 */
            Syscall2();
        }
        /* assign exception values */
        currentProcess->p_oldSys = (state_t *)caller->s_a2;
        currentProcess->p_newSys = (state_t *)caller->s_a3;
    }

    LDST(&(caller));
}

/*Syscall6:  "Get_CPU_Time"
    This service is in charge of making sure that the amount of time spent being processed is tracked by 
    each Process Block that is running. 
        Parameters: State_t * caller
        Return: Void*/
HIDDEN void Syscall6(state_t *caller)
{
    cpu_t timeSpentProcessing;
    STCK(timeSpentProcessing);

    /*Track the amout of time spent processing and add this to the previous amount of process time*/
    (currentProcess->p_timeProc) = (currentProcess->p_timeProc) + (timeSpentProcessing - TODStart);
    /*Store the new updated time spent processing into the v0 register of the process state*/
    (caller->s_v0) = (currentProcess->p_timeProc);

    /*Updates start time*/
    STCK(TODStart);
    /*Load the Current Processes State*/
    LDST(caller);
}

/*  Syscall 7 performs a syscall 4 on the Semaphore associated to clock timer
    Knowing that this clock also has a syscall 3 performing on it every 100 milliseconds
    Parameters: State_t* Caller
    Return: Void*/
HIDDEN void Syscall7(state_t *caller)
{
    int *sem;
    sem = (int *)&(semD[SEMNUM - 1]);
    (*sem)--;

    if (sem < 0)
    {
        /*Sem is less than 0 block the current process*/
        insertBlocked(sem, currentProcess);
        CtrlPlusC(caller, &(currentProcess->p_s));
        /*Increment that we have another process soft block so that it does not starve*/
        softBlockCount++;
    }
    fuckme(1616);
    /*Process is soft blocked call to run another process*/
    scheduler();
}

/*  This service perofroms a Syscall 5 operation on the semaphore that the nucles maintains for the IO 
    device by the values in a1, a2, and a3 (optionally). The nucleus performs a V operation on the
    semaphore whenever that device generates an interrupt. 
    Return:     Device Status in v0 (Once the process resumes after the occurrence of the anticipated
                interrupt)*/
HIDDEN void Syscall8(state_t *caller)
{

    fuckme(2);
    int lineNo; /*  line number*/
    int dnum;   /*  device number*/
    int termRead;
    int index;
    int *sem;

    lineNo = caller->s_a1;
    dnum = caller->s_a2;
    termRead = caller->s_a3; /* terminal read  or write */

    /* what device is going to be computed*/

    if (lineNo < DISKINT || lineNo > TERMINT)
    {
        Syscall2();
    }

    if (lineNo == TERMINT && termRead == TRUE)
    {
        fuckme(4);
        /* terminal read */
        index = DEVPERINT * (lineNo - DEVWOSEM + termRead) + dnum;
    }

    else
    {
        fuckme(34);
        /* anything else */
        index = DEVPERINT * (lineNo - DEVWOSEM) + dnum;
    }

    sem = &(semD[index]);
    (*sem)--;

    if ((*sem) < 0)
    {
        insertBlocked(sem, currentProcess);
        CtrlPlusC(caller, &(currentProcess->p_s));

        softBlockCount++;

        /*DECIDED TO CALL SCHEDULER instead of giving back time to the process that was interrupted
        Keeps the overall flow of the program and since there is no starvation, eventually that process
        will get its turn to play with the processor*/
        /*LDST(caller);*/
        fuckme(30);
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
    LoadState(&newState);
}

/*Gets triggered when the executing process performs an illegal operation. Therefore, since  this is 
    triggered when a PgmTrap exception is raised, execution continues with the nucleus’s PgmTrap exception
    handler. The cause of the PgmTrap exception will be set in Cause.ExcCode in the PgmTrap Old Area.
    Parameters: None
    Return: Void
     */
void PrgTrapHandler()
{
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
    while (!emptyChild(headPtr))
    {
        /*We are going to the bottom most child to KILL every child in list (Rinse and Repeat)*/
        NukeThemTillTheyPuke(removeChild(headPtr));
    }

    if (headPtr == currentProcess)
    {
        /*  Children services comes for you and take your child*/
        outChild(currentProcess);
    }
    if ((headPtr->p_semAdd) == NULL)
    {
        /*  remove process from readyQueue*/
        outProcQ(&readyQue, headPtr);
    }
    else
    {
        int *sema4 = (headPtr->p_semAdd);
        /*  remove process from ASL*/
        outBlocked(headPtr);
        if ((sema4 >= &(semD[0])) && (sema4 <= &(semD[SEMNUM - 1])))
        { /*SemAdd count is somewhere in between the SemD array*/
            softBlockCount--;
        }
        else
        {
            /*  Increment Semaphore*/
            (*sema4)++;
        }
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

/*Function that is designed for ME to be able to read that LDST is Load State 
Parameters: state_t * s
    Return: Void*/
HIDDEN void LoadState(state_t *s)
{
    LDST(s);
}

/*Track the Time
    Parameters: Cpu_t t
    Return: Void*/
HIDDEN void StoreTime(cpu_t t)
{
    STCK(t);
}
