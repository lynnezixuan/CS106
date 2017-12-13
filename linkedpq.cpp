/* This is the cpp file for patient queue, managing a waiting line of patients
 * to be processed at a hospital.
 * Refer to the code on the ppt from lecture15.
 *
 * by Zixuan Zhou
 */

#include "linkedpq.h"
/* constructor function: initialize front; O(1)
 */
LinkedPQ::LinkedPQ() {

    PQ_front = NULL;
}
/* deconstructor function: delete the dynamic distributed space to avoid memory leak; O(N)
 */
LinkedPQ::~LinkedPQ() {
    clear();
}
/* clear by user : remove all node
 * delete the dynamic distributed space to avoid memory leak; O(N)
 */
void LinkedPQ::clear() {
    PNode* current = PQ_front;// delete from front to the back successively
    while ( current != NULL ){
        PQ_front = PQ_front -> next;
        delete current;
        current = PQ_front;
    }
    // clear the front point
    PQ_front = NULL;
}
/* peek function: get the front name; O(1)
 * const: will not change the structure of the list
 */
string LinkedPQ::frontName() const {
    string name ="";
    //if list is empty, then throw exception
    //if not, return the name
    if(isEmpty())
        throw "exception";
    else{
        name = PQ_front->name;
    }
    return name;
}
/* peek function: get the front priority; O(1)
 * const: will not change the structure of the list
 */
int LinkedPQ::frontPriority() const {
    int priority = 0;
    //if list is empty, then throw exception
    //if not, return the first priority
    if(isEmpty())
        throw "exception";
    else{
        priority = PQ_front->priority;
    }
    return priority;   // this is only here so it will compile
}
/* if the list is empty, return ture; O(1)
 * const: will not change the structure of the list
 */
bool LinkedPQ::isEmpty() const {
     if ( PQ_front == NULL )// front is null and thus list is empty
         return true;
     else
         return false;
}
/* insert the node into the list according to its priority, O(N)
 *
 */
void LinkedPQ::newPatient(string name, int priority) {
    if( PQ_front == NULL )//if the list is empty, add the node to the list
        PQ_front = new PNode( name, priority,NULL);
    //add the node to the front of the list
    else if ( priority < PQ_front->priority ){
        PQ_front = new PNode( name, priority,PQ_front);
    }else{
        PNode* current = PQ_front;
        bool find_pos = current->next != NULL && priority >= current->next-> priority;
        //find the position for inserting the node
        while ( find_pos ){
            current = current ->next;
            find_pos = current->next != NULL && priority >= current->next-> priority;
        }
        current->next = new PNode( name, priority, current->next);
    }
}
/* remove the front node fromt the list and return his or her name;O(1)
 */
string LinkedPQ::processPatient() {
    string name ="";
    //if list is empty, then throw exception
    if(isEmpty())
        throw "exception";
    else{
        PNode* Proccessed = PQ_front;
        PQ_front = PQ_front->next;//front point to next node
        name = Proccessed->name;
        delete Proccessed;// delete the dynamic distributed space
    }
    return name;
}
/*  remove the old node, add a new node with new priority to the list; O(N)
 */
void LinkedPQ::upgradePatient(string name, int oldPriority, int newPriority) {
    PNode* current = PQ_front;
    PNode* temp;
    bool find_old = (current->priority != oldPriority || current->name != name)&&current != NULL;
    //find the old postition
    while ( find_old)
    {
        temp = current;
        current = current->next;
        find_old = (current->priority != oldPriority || current->name != name)&&current != NULL;
    }
    //if we cannot find the node in the list or the priority increases, then throw exception
    if ( oldPriority <= newPriority||current==NULL )
        throw "exception";
    // remove the old node, add a new node with new priority to the list
    else {
        temp->next = current->next;
        delete current;
        newPatient(name,newPriority);
    }
}
/* unused
 */
void LinkedPQ::debug() {
    // TODO: write this function (optional)
}
/* output the node list; O(N)
 */
ostream& operator <<(ostream& out, const LinkedPQ& queue) {
    out <<"{";
    //if list is empty, output {}
    //if not, output whole list succesively
    if ( queue.PQ_front != NULL ){
        PNode* current = queue.PQ_front;
        while ( current!= NULL){
            out << current->priority << ":" << current->name;
            if (current->next != NULL)// last node will not followed by a comma
                out << ", ";
            current = current->next;
        }
    }
    out <<"}";
    return out;
}
