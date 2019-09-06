
/*  PHASE 1
    NICK STONE AND SANTIAGO SALAZAR
    PROFESSOR MIKEY G */

/*********************************************************************************************
                            Module Comment Section
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



/*   If the Head Node is NULL then the list is empty and we return NULL*/

HIDDEN semd_t *allocASL(){
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

/*  This function goes through the pcb*/
HIDDEN semd_t *searchForParent(int *semAdd){
	semd_t *temp = semd_h;
    
	if(semAdd == NULL){
        semAdd = MAXINT; 
    }
	while(semAdd > (temp -> s_next -> s_semAdd)){
		temp = temp -> s_next;
        
	}
    
	return temp;
}

/*  */
HIDDEN void deAllocASL(semd_t *s){
    if(semdFree_h==NULL){
        semdFree_h = s;
        semdFree_h -> s_next = NULL;
    }else{
        s->s_next = semdFree_h;
        semdFree_h = s;
    }
}
