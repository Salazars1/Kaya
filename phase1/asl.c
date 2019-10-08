
/* PHASE 1
    Written by NICK STONE AND SANTIAGO SALAZAR
    Base code and Comments from PROFESSOR MIKEY G 
    Finished on 9/5/2019 */

/*********************************************************************************************
                            Module Comment Section
asl.c is a program that is responsible to maintainning and creating the active semaphore list. 
It is important to know that this module uses Pcb.c but also uses the structs pcb_t and semd_t
from types.h and a few constants from const.h. This program also has a few helper functions that 
are used thoughout the ASL and are actively called by each function. In the high level the ASL 
is responsible for allocating and freeing semd_t and maintains two singley linked NUll terminated
linked lists. The active semaphore list contains two sentinnel nodes
 It is important to note that in this module we will refer to Semd_t structs as Nodes 
and that pcb_t may be referred to as Procblks and represent process blocks. Each active semaphore 
list has a corresponding doubley linked circular tail pointer data structure of process blocks
**********************************************************************************************/
#include "../h/const.h"
#include "../h/types.h"
#include "../e/pcb.e"


HIDDEN semd_t *semd_h;                  /* Globally defines active list*/
HIDDEN semd_t *semdFree_h;              /* Globally define free semaphore list */



/*  Insert the ProcBlk pointed to by p at the tail of the process queue associated 
    with the semaphore (semAdd) and sets the semaphore address of p to semAdd. 
    Parameteres: semAdd, pcb_t
    Return:     -False (if the node was inserted succesfully and the lists where updated )
                -True (if semFree is empty and the node wasnt inserted sucesfully)*/

int insertBlocked(int *semAdd, pcb_t *p){
    semd_t * temp;
    temp = searchForParent(semAdd);
    if(temp -> s_next -> s_semAdd == semAdd){           /*ID is in the ASL*/
        p->p_semAdd = semAdd;
        insertProcQ(&(temp->s_next->s_procQ),p);        /*Calls pcb to insert pcb*/  
        return FALSE;
    }else{                                              /*ID is not in the ASL*/
        semd_t *newSemd = allocASL();                   /*Creates new node*/
        if(newSemd == NULL){
            return TRUE;                                /*More than 20 (MAXPROC) Processes*/
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

/*  Searches the ASL list for a descriptor of this semaphore and returns the pointer
    to the the removed node. It removes the node by using removeProcQ from pcb. 
    Parameters: semAdd
    Return:     - NULL (if no node with the semAdd is found)
                - pcb_t (if the node was found in the list)*/

pcb_t *removeBlocked(int *semAdd){
    semd_t *parentNode;
    parentNode = searchForParent(semAdd);                 /*Gets the parent of the node whose semAdd equals the parameters*/

    pcb_t * returnValue;
    if(parentNode -> s_next -> s_semAdd == semAdd){       /*ID is in the ASL*/
        returnValue  = removeProcQ(&parentNode ->s_next ->s_procQ);
        if(returnValue == NULL){                          /*SemAdd was not found in the list*/
            return NULL;  
        }

        if(emptyProcQ(parentNode ->s_next ->s_procQ)){    /*Fixes pointers*/
            semd_t *removedNode = parentNode -> s_next; 
            parentNode -> s_next = parentNode -> s_next -> s_next;
            deAllocASL(removedNode);
        }
        
        returnValue -> p_semAdd = NULL;                     /*semAdd in node is not neccessary*/
        return returnValue;
    
    }else{   
        return NULL;
    }
}

/*  Searches the ASL list for an specifi pcb node pointer and returns the pointer to the the 
    removed node. It removes the node by using outprocQ from pcb. 
    Parameters: semAdd
    Return:     - NULL (if no node with the semAdd is found)
                - pcb_t (if the node was found in the list)*/
pcb_t *outBlocked(pcb_t *p){

    semd_t *parentNode;
    parentNode = searchForParent(p->p_semAdd);                 /*Gets the parent of the node whose semAdd equals the parameters*/   
    pcb_t * returnValue;

    if(parentNode -> s_next -> s_semAdd == p->p_semAdd){       /*ID is in the ASL*/
        returnValue  = outProcQ(&(parentNode ->s_next ->s_procQ) ,p);
        if(returnValue == NULL){                               /*SemAdd was not found in the list*/ 
            return NULL; 
        }

        if(emptyProcQ(parentNode ->s_next ->s_procQ)){         /*Fixes pointers*/ 
            semd_t *removedNode = parentNode -> s_next; 
            parentNode -> s_next = parentNode -> s_next -> s_next;
            deAllocASL(removedNode);
        }
        
        returnValue -> p_semAdd = NULL;                         /*semAdd in node is not neccessary*/
        return returnValue;
    
    }else{
        return NULL;
    }

}

/*  Returns a pointer to the ProcBlk that is at the head of the process queue associated with
    the semaphore semAdd. It uses headProckQ in pcb
    Parameters: semAdd
    Return:     - NULL (if semAdd is not found on the ASL)
                - NULL  (if the process queue of semAdd is empty) 
                - pcb_t* (if the was found in the list)*/

pcb_t *headBlocked(int *semAdd){
    
	semd_t *temp;
    temp = searchForParent(semAdd);                         /*Gets the parent of the node whose semAdd equals the parameters*/
   
   if(temp->s_semAdd == NULL){                              /*New node doesnt have a semAdd*/
       return NULL; 
   }else if(temp ->s_next ->s_procQ == NULL){               /*Node whose semAdd equals the parameter and s_procQ is NULL*/
        return NULL; 
    } 

    return headProcQ(temp->s_next->s_procQ);            
}

/*Initialize the semdFree list to contain all the elements of the list*/
initASL(){
    static semd_t ASLInitialization[MAXPROC+2];                 /*Sets up the size to include the two dummie nodes*/
    
    semd_h = NULL; 
    semdFree_h = NULL;
    int i;
    for(i = 2; i < MAXPROC+2; i++){
        deAllocASL(&(ASLInitialization[i]));
    }

    semd_t *firstSent = &ASLInitialization[0];                  /*Sets the first dummy node to be the first node ([0]) in the arary*/
    semd_t *lastSent = &ASLInitialization[1];                   /*Sets the first dummy node to be the second node ([1]) in the arary*/

    firstSent ->s_semAdd = NULL;                                /*First Dummy node semAdd is 0*/
    lastSent -> s_semAdd = MAXINT;                              /*Last dummy node semAdd is 0xFFFFFFFF*/
    firstSent ->s_next = lastSent;                              
    lastSent -> s_next = NULL;                                  
    firstSent -> s_procQ = NULL;                                
    lastSent ->s_procQ = NULL;                                  

    semd_h = firstSent;                                         /*Sets semd List to include de dummy nodes */
}


/******************************************HELPER FUCTION**********************************************/
/*  This set of functions despite not having a crucial importance are created to simplify code
    (easier to read) and avoid repeating the same code over and over again*/



/*  Helper Function to allocate values in ASL (likewise in pcb). This sets everything within the node pointer
    semd_t.
    Parameter:
    Return:     -NULL (If the Head Node is is empty)*/

HIDDEN semd_t *allocASL(){
    semd_t * temp;

    if(semdFree_h == NULL){
        return NULL;
    }

    /* We then make a temporary Pointer to the head of the free list. Then we move the head pointer in the free
    list to be the next process in the list. Then we set all of the temporary pointers values to NULL or 0 
    */

    temp = semdFree_h;
    semdFree_h = semdFree_h-> s_next;

    temp->s_next = NULL; 
    temp->s_semAdd = NULL; 
    temp->s_procQ = mkEmptyProcQ(); 

    return temp;                                /*Returns the created node*/
}

/*  This function is a helper function goes that through the asl node to determine if the next node has 
    semAdd equal to the one in the parameter. To keep the list sorted compares whether next semAdd is greater
    than the semd_t sempahore address. If semadd is null, then it sets it up to be the 0xFFFFFFF.
    Parameter: semAdd
    Return:         -semAdd parent: if semAdd is found
                    -dummy node parent: if semAdd is not found.*/
    
HIDDEN semd_t *searchForParent(int *semAdd){
	semd_t *temp = semd_h;
    
	if(semAdd == NULL){
        semAdd = MAXINT; 
    }

	while(semAdd > (temp -> s_next -> s_semAdd)){               /*Checks the semaphore with an greater than sign to keep the list organized*/
		temp = temp -> s_next;   
	}
    
	return temp;
}

/*  Helper Function to deallocate values in ASL (likewise in pcb). This adds the nodes into the free semaphore
    list, to keep track of how many free semaphore we have. Note, this process depends inderectly on the MAX PROC
    value. 
    Parameter:  semd_t
    Return:     */
HIDDEN void deAllocASL(semd_t *s){
    if(semdFree_h==NULL){                           /*semdFreeList was empty*/
        semdFree_h = s;                             /*adds the node pointer to semdFreeList*/
        semdFree_h -> s_next = NULL;                
    }else{                                          /*semdFreeList was empty*/
        s->s_next = semdFree_h;                     
        semdFree_h = s;                             /*adds the node pointer to semdFreeList*/
    }
}
