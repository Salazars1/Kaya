
/*  PHASE 1
    NICK STONE AND SANTIAGO SALAZAR
    PROFESSOR MIKEY G */

/*********************************************************************************************
                            Module Comment Section
**********************************************************************************************/
#include "../h/const.h"
#include "../h/types.h"
#include "../e/pcb.e"


HIDDEN semd_t *semd_h;                  /* active list*/
HIDDEN semd_t *semdFree_h;              /* free semaphore list */



/* Insert the ProcBlk pointed to by p at the tail of the process queue
associated with the semaphore whose physical address is semAdd
and set the semaphore address of p to semAdd. 

If the semaphore is currently not active (i.e. there is no descriptor for it in the ASL), 
allocate a new descriptor from the semdFree list, insert it in the ASL (at
12 CHAPTER 2. PHASE 1 - LEVEL 2: THE QUEUES MANAGER
the appropriate position), initialize all of the fields (i.e. set s semAdd
to semAdd, and s procq to mkEmptyProcQ()), and proceed as
above. If a new semaphore descriptor needs to be allocated and the
semdFree list is empty, return TRUE. In all other cases return FALSE.
*/

int insertBlocked(int *semAdd, pcb_t *p){
    semd_t * temp;
    temp = searchForParent(semAdd);
    if(temp -> s_next -> s_semAdd == semAdd){         /*ID is in the ASL*/
        p->p_semAdd = semAdd;
        insertProcQ(&(temp->s_next->s_procQ),p);       
        return FALSE;
    }else{                                          /*ID is not in the ASL*/
        semd_t *newSemd = allocASL();              /*Create new node*/
        if(newSemd == NULL){
            return TRUE;            /*More than 20 (MAXPROC) Processes*/
        }else{
            newSemd -> s_next = temp -> s_next;
            temp->s_next = newSemd;
            newSemd->s_procQ = mkEmptyProcQ();

            p->p_semAdd = semAdd;
            newSemd -> s_semAdd = semAdd;
            insertProcQ(&(temp->s_next->s_procQ),p);       
            return FALSE;
        }

    }
}

/* Search the ASL for a descriptor of this semaphore. If none is
found, return NULL; otherwise, remove the first (i.e. head) ProcBlk
from the process queue of the found semaphore descriptor and return a pointer to it. If the process queue for this semaphore becomes
empty (emptyProcQ(s procq) is TRUE), remove the semaphore
descriptor from the ASL and return it to the semdFree list. */

pcb_t *removeBlocked(int *semAdd){
    semd_t *parentNode;
    parentNode = searchForParent(semAdd);

    pcb_t * returnValue;

   
    if(parentNode -> s_next -> s_semAdd == semAdd){       /*ID is in the ASL*/
  
        returnValue  = removeProcQ(&parentNode ->s_next ->s_procQ);
        if(emptyProcQ(parentNode ->s_next ->s_procQ)){    /*Need to fix pointers*/
            semd_t *testing = parentNode -> s_next; 
            parentNode -> s_next = parentNode -> s_next -> s_next;
            freeASL(testing);
        }
        returnValue -> p_semAdd = NULL;             /*semAdd in node is not neccessary*/
        return returnValue;
    }else{
       
        return NULL;
    }
}

/* Remove the ProcBlk pointed to by p from the process queue associated with p’s semaphore (p→ p semAdd) on the ASL. If ProcBlk
pointed to by p does not appear in the process queue associated with
p’s semaphore, which is an error condition, return NULL; otherwise,
return p. */

pcb_t *outBlocked(pcb_t *p){

    semd_t *parentNode;
    parentNode = searchForParent(p->p_semAdd);

    pcb_t * returnValue;

   
    if(parentNode -> s_next -> s_semAdd == p->p_semAdd){       /*ID is in the ASL*/
  
        returnValue  = outProcQ(&parentNode ->s_next ->s_procQ,p);
        if(emptyProcQ(parentNode ->s_next ->s_procQ)){    /*Need to fix pointers*/
            semd_t *testing = parentNode -> s_next; 
            parentNode -> s_next = parentNode -> s_next -> s_next;
            freeASL(testing);
        }
        returnValue -> p_semAdd = NULL;             /*semAdd in node is not neccessary*/
        return returnValue;
    }else{
       
        return NULL;
    }

}

/* Return a pointer to the ProcBlk that is at the head of the process queue associated with the semaphore semAdd. Return NULL
if semAdd is not found on the ASL or if the process queue associated with semAdd is empty. */

pcb_t *headBlocked(int *semAdd){
    semd_t * temp;
    temp = searchForParent(semAdd);
    if(temp->s_next ->s_procQ == NULL){
        return NULL;
    }
    else{
        return headprocQ(temp -> s_next -> s_procQ);
    }
}

/*Initialize the semdFree list to contain all the elements of the array*/


initASL(){
    static semd_t ASLInitialization[MAXPROC+2];
    
    semd_h = NULL; 
    semdFree_h = NULL;
    int i;
    for(i = 2; i < MAXPROC+2; i++){
        freeASL(&(ASLInitialization[i]));
    }
    semd_t *firstSent = &ASLInitialization[0];
    semd_t *lastSent = &ASLInitialization[1];
    firstSent ->s_semAdd = NULL; 
    lastSent -> s_semAdd = MAXINT; 
    firstSent ->s_next = lastSent; 
    lastSent -> s_next = NULL; 
    firstSent -> s_procQ = NULL;
    lastSent ->s_procQ = NULL; 

    semd_h = firstSent; 
    




}






/************************HELPER FUCTION******************************/

HIDDEN semd_t *allocASL(){
   /*   If the Head Node is NULL then the list is empty and we return NULL*/
    
    semd_t * temp;
    
    if(semdFree_h == NULL){
        return NULL;
    }
    /* We then make a temporary Pointer to the head of the free list 
    Then we move the head pointer in the free list to be the next process in the list
    Then we set all of the temporary pointers values to NULL or 0 
    Return Temp
    */

    temp = semdFree_h;
    semdFree_h = semdFree_h-> s_next;

    temp->s_next = NULL; 
    temp->s_semAdd = NULL; 
    temp->s_procQ = mkEmptyProcQ(); 

    return temp; 
}


HIDDEN semd_t *searchForParent(int *semAdd){
	semd_t *temp = semd_h;
	
	while(semAdd > (temp -> s_next -> s_semAdd)){
		temp = temp -> s_next;
        
	}
	return temp;
}


HIDDEN void freeASL(semd_t *s){
    if(semdFree_h==NULL){
        semdFree_h = s;
        semdFree_h -> s_next = NULL;
    }else{
        s->s_next = semdFree_h;
        semdFree_h = s;
    }
}
