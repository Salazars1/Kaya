// PHASE 1
// NICK STONE AND SANTIAGO SALAZAR
// PROFESSOR MIKEY G.

/*********************************************************************************************
                            Module Comment Section







**********************************************************************************************/

//Globally defines pcbList_h.
static pbc_t *pcbList_h;

/*  Insert the element pointed to by p into the pcbFree list
    Parameters: pcb_t *p
    Return: void*/ 
void freePcb(pcb_t *p){
    p->next = pcbList_h;
    pcbList_h = p;
}

/*  Removes an element from the pcbFree list, provide initial values for ALL of 
    the ProcBlk’s fields (i.e. NULL and/or 0)
    Parameters:
    Return: -Null (if the pcbFree list is empty)
            -Pointer to the removed element (if the pcbFree list is NOT EMPTY)*/
//TODO:
pcb_t *allocPcb(){
    if(pcbList_h == NULL){
        return NULL;
    }
    pcb_t * temp = pcbList_h; // TODO: return the whole node? ptr?
    pcbFree_h = pcbFree_h-> next;
    return tempPtr; 
}

/*  Initialize the pcbFree list to contain all the elements of the static array of
    MAXPROC ProcBlk’s.*/
TODO:
initPcbs(){
    /*  
    Set the pcbFree_h to be NULL

    Then We set the pcbFree_h to be the first item in the array 
    Then we loop through the array knowing that it is static so we know the size of the array
    Then We create a temp pointer (Pcb_t * temp) to be pcbFree_h -> next

    Keep setting the next value to be the next element in the array


    No Return Value 
    We are done 
    */
}

/* This method is used to initialize a variable to be tail pointer to a
    process queue.
    Return a pointer to the tail of an empty process queue; i.e. NULL. */
pcb_t *mkEmptyProcQ(){
    return NULL; 
}

/*  Checks wether the tail of the queue is empty or not. 
    Paramater: pcb_t 
    Return  TRUE (if the queue whose tail is pointed to by tp is empty.)
            FALSE (if the queue whose tail is pointed to by tp is NOT empty.)*/
int emptyProcQ(pcb_t *tp){
    return (tp == NULL);
}

/*  Inserts the ProcBlk pointed to by p into the process queue
    Parameters: **tp, *p
    Return: Void*/
void insertProcQ(pcb_t **tp, pcb_t *p){

    if(emptyProQ(*tp)){             //Case 1: There is no node. 
        p->NEXT = p;
        p->PREV = p;
    }else if(tp->NEXT == (*tp)){    //Case 2: There is only one node. 
        p->NEXT = *tp;
        p->PREV = *tp;
        (*tp)->PREV = p;
        (*tp)->NEXT = p; 
    }else{                          //Case 3: There is more than one node.
        pcb_t tempHead = tp -> NEXT;    //Initialize the Queue head
        tp -> NEXT = p;                 //Adds the new node
        p -> NEXT = tempHead;           //Fixes Pointers  
        tempHead -> PREV = p;
        p -> PREV = tp;
        tp = p; 
    }
}

/*  Remove the first (i.e. head) element from the process queue
    Parameters: **tp
    Return: NULL    (if the process queue is empty)
            pcb_t   (if the process queue is NOT empty)*/
pcb_t *removeProcQ(pcb_t **tp){
    if(emptyProcQ(*tp)){                    //Case 1: Queue is empty
        return NULL;
    }else if(){                             //Case 2: Queue has 1 nodes
        pcb_t temp = *tp;
        *tp = mkEmptyProcQ();
        return temp;
    }else{                                  //Case 3: Queue has 2 or more nodes
        pcb_t temp = tp-> NEXT;
        tp -> NEXT = temp-> NEXT;
        temp -> NEXT -> PREV = tp;
        return temp;
    }
}

TODO:
pcb_t *outProcQ(pcb_t **tp, pcb_t *p){
    if(tp==p){
        
    }else{
        Pcb_t * temp = tp; 
        while(temp != NULL){        //Loop through the linked list
            if(temp == p){
                return temp
            }else{
                temp = temp -> next; 
            }
        }
        Process Not in list
        return NULL
        

    }
}


/*  Checks wether the process queue has more than one node.
    Parameters: pcb-t * tp
    Return: NULL     (if the process queue is empty)
            tp->Next (if the process queue is NOT empty). */
pcb_t *headProcQ(pcb_t *tp){
    if(emptyProQ){
        return NULL;
    }
    return tp->NEXT;
}




