// PHASE 1
// NICK STONE AND SANTIAGO SALAZAR
// PROFESSOR MIKEY G.

/*********************************************************************************************
                            Module Comment Section
**********************************************************************************************/

//Globally defines pcbList_h.
HIDDEN pbc_t *pcbList_h;

/*  Insert the element pointed to by p into the pcbFree list
    Parameters: pcb_t *p
    Return: void*/ 
void freePcb(pcb_t *p){
    /*
    Set the Node we are addings Next pointer to the current head
    Set head to be the new node 
    */
    p->p_next = pcbList_h;
    pcbList_h = p;
}

/*  Removes an element from the pcbFree list, provide initial values for ALL of 
    the ProcBlk’s fields (i.e. NULL and/or 0)
    Parameters:
    Return: -Null (if the pcbFree list is empty)
            -Pointer to the removed element (if the pcbFree list is NOT EMPTY)*/

pcb_t *allocPcb(){
   /*If the Head Node is NULL then the list is empty and we return NULL*/
    if(pcbList_h == NULL){
        return NULL;
    }
    /* We then make a temporary Pointer to the head of the free list 
    Then we move the head pointer in the free list to be the next process in the list
    Then we set all of the temporary pointers values to NULL or 0 
    Return Temp
    */
    pcb_t * temp = pcbList_h;
    pcblist_h = pcblist_h-> p_next;

    temp -> p_child = NULL; 
    temp -> p_nextSib = NULL; 
    temp -> p_prevSib = NULL; 
    temp -> p_prnt = NULL; 
    temp -> p_next = NULL; 
    temp -> p_prev = NULL; 
    return temp; 
}

/*  Initialize the pcbFree list to contain all the elements of the static array of
    MAXPROC ProcBlk’s.
    
        Set the pcbFree_h to be NULL
        Then We set the pcbFree_h to be the first item in the array 
        Then we loop through the array knowing that it is static so we know the size of the array
        Then We create a temp pointer (Pcb_t * temp) to be pcbFree_h -> p_next
        Keep setting the p_next value to be the p_next element in the array
        Return:  
    */

initPcbs(){

    static pcb_t Pcbinitialization[MAXPROC];
    pcbList_h = NULL;
    for(i = 0; i < MAXPROC(10); i++){
        freePcb(&(pcbinitialization[i]));
    }

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

    if(emptyProQ(*tp)){                 //Case 1: There is no node. 
        p->p_next = p;
        p->p_prev = p;
    }else if(tp->p_next == (*tp)){      //Case 2: There is only one node. 
        p->p_next = *tp;
        p->p_prev = *tp;
        *tp->p_prev = p;
        *tp->p_next = p; 
    }else{                              //Case 3: There is more than one node.
        pcb_t tempHead = *tp -> p_next;     //Initialize the Queue head
        *tp -> p_next = p;                  //Adds the new node
        p -> p_next = tempHead;             //Fixes Pointers  
        tempHead -> p_prev = p;
        p -> p_prev = tp;
        *tp = p; 
    }
}

/*  Remove the first (i.e. head) element from the process queue
    Parameters: **tp
    Return: NULL    (if the process queue is empty)
            pcb_t   (if the process queue is NOT empty)*/
pcb_t *removeProcQ(pcb_t **tp){
    if(emptyProcQ(*tp)){                    //Case 1: Queue is empty
        return NULL;
    }else if(tp->p_next == (*tp)){            //Case 2: Queue has 1 nodes
        pcb_t temp = *tp;
        *tp = mkEmptyProcQ();
        return temp;
    }else{                                  //Case 3: Queue has 2 or more nodes
        pcb_t temp = *tp-> p_next;
        *tp -> p_next = temp-> p_next;
        temp -> p_next -> p_prev = *tp;
        return temp;
    }
}

TODO:
pcb_t *outProcQ(pcb_t **tp, pcb_t *p){
    if(*(tp)==p){
        

        
    }else{
        Pcb_t * temp = *tp; 
        while(temp != NULL){        //Loop through the linked list
            if(temp == p){
                return temp
            }else{
                temp = temp -> p_next; 
            }
        }
        Process Not in list
        return NULL
        

    }
}


/*  Checks wether the process queue has more than one node.
    Parameters: pcb-t * tp
    Return: NULL     (if the process queue is empty)
            tp->p_next (if the process queue is NOT empty). */
pcb_t *headProcQ(pcb_t *tp){
    if(emptyProQ()){
        return NULL;
    }
    return tp->p_next;
}


//************************* Process Tree Maintenance *************************************



/*  Checks wether the Blok queue has Children or not.
    Parameters: pcb-t * p
    Return: True        (if the proBlk has no Children)
            False       (if the proBlk has no Children). */
int emptyChild(pcb_t *p){
    return (p->p_child== NULL)
}


/*  Make the ProcBlk pointed to by p a child of the ProcBlk pointed to by prnt
    Parameters: pcb-t * p
    Return: void*/
void insertChild(pcb_t *p_prnt, pcb_t *p){
    if(emptyChild(p_prnt)){               //There is no childre
        p_prnt -> p_child= p;
        p -> p_prnt = p_prnt;
        p -> p_prevSib = NULL;
        p -> p_nextSib = NULL;
    }else{                              //There is 1 or more children
        p -> p_prnt = *p_prnt;
        p_prnt -> p_child-> p_prevSib = p;
        p -> p_nextSib = p_prnt -> child;
        p -> p_prevSib = NULL;
        p_prnt ->  p_child= p;
    }
}


/*  Make the first child of the ProcBlk pointed to by p no longer a
    child of p.
    Parameters: pcb_t
    Return: Null        (if there is no children)
            pcb_t * p   (to the removed child of ProcBlk)*/
pcb_t *removeChild(pcb_t *p){
    pcb_t temp;
    if(emptyChild(p)){                             //No Children
        return NULL;   
    }else if(p -> p_child-> p_nextSib == NULL){       //One Child
        temp = p -> child;
        temp -> p_prnt = NULL;
        temp -> p_nextSib = NULL;
        temp -> p_prevSib = NULL;
        p -> p_child= NULL;

        return temp;                                //More than one children
    }else{
        temp = p->child;
        temp -> p_nextSib -> p_prevSib = NULL;
        p-> p_child= temp -> p_nextSib;
        temp -> p_nextSib = NULL;
        temp -> p_prnt = NULL;
        
        return temp;
    }
}

TODO:
/* Make the ProcBlk pointed to by p no longer the child of its parent.
    If the ProcBlk pointed to by p has no parent, return NULL; otherwise,
    return p. Note that the element pointed to by p need not be the first
    child of its parent. */
pcb_t *outChild(pcb_t *p){

}

