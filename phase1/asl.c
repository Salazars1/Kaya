
/*  PHASE 1
    NICK STONE AND SANTIAGO SALAZAR
    PROFESSOR MIKEY G */

/*********************************************************************************************
                            Module Comment Section
**********************************************************************************************/
#include "../h/const.h"
#include "../h/types.h"
#include "../e/pcb.e"


HIDDEN semd_t *semd_h;                  /* */
HIDDEN semd_t *semdFree_h;              /* free semaphore list */



/* Insert the ProcBlk pointed to by p at the tail of the process queue
associated with the semaphore whose physical address is semAdd
and set the semaphore address of p to semAdd. If the semaphore is
currently not active (i.e. there is no descriptor for it in the ASL), 
allocate a new descriptor from the semdFree list, insert it in the ASL (at
12 CHAPTER 2. PHASE 1 - LEVEL 2: THE QUEUES MANAGER
the appropriate position), initialize all of the fields (i.e. set s semAdd
to semAdd, and s procq to mkEmptyProcQ()), and proceed as
above. If a new semaphore descriptor needs to be allocated and the
semdFree list is empty, return TRUE. In all other cases return FALSE.
*/
int insertBlocked(int *semAdd, pcb t *p){

}

/* Search the ASL for a descriptor of this semaphore. If none is
found, return NULL; otherwise, remove the first (i.e. head) ProcBlk
from the process queue of the found semaphore descriptor and return a pointer to it. If the process queue for this semaphore becomes
empty (emptyProcQ(s procq) is TRUE), remove the semaphore
descriptor from the ASL and return it to the semdFree list. */
pcb_t *removeBlocked(int *semAdd){


}

/* Remove the ProcBlk pointed to by p from the process queue associated with p’s semaphore (p→ p semAdd) on the ASL. If ProcBlk
pointed to by p does not appear in the process queue associated with
p’s semaphore, which is an error condition, return NULL; otherwise,
return p. */
pcb_t *outBlocked(pcb t *p){


}

/* Return a pointer to the ProcBlk that is at the head of the process queue associated with the semaphore semAdd. Return NULL
if semAdd is not found on the ASL or if the process queue associated with semAdd is empty. */
pcb_t *headBlocked(int *semAdd){


}

/*Initialize the semdFree list to contain all the elements of the array*/
initASL(){


}

