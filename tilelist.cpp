/* This is the cpp file for tilelist, window manager of your operating system.
 * Refer to the code on the ppt from lecture15.
 *
 * by Zixuan Zhou
 */
#include "tilelist.h"
/* constructor function: initialize front and back; O(1)
 */
TileList::TileList() {
    front = NULL;
    back = NULL;
}
/* deconstructor function: delete the dynamic distributed space to avoid memory leak; O(N)
 */
TileList::~TileList() {
    clear();
}
/* unused
 */
void TileList::debug() {
    // TODO: write this function (optional)
}
/* add a node to the back of the list; O(1)
 */
void TileList::addBack(int x, int y, int width, int height, string color) {
    TileNode* current = back;// start from the back to the front
    back = new TileNode( x, y, width, height, color, back, NULL );
    // if the list is empty, we have to set the point front;
    // if not, front will not change, and the next of original back is set to the current back
    if ( front == NULL)
        front = back;
    else
        current->next = back;
}
/* add a node to the front of the list; O(1)
 */
void TileList::addFront(int x, int y, int width, int height, string color) {
    TileNode* current = front;// start from the front to the back
    front = new TileNode( x, y, width, height, color, NULL, front );
    // if the list is empty, we have to set the point back;
    //  if not, back will not change, and the prev of original front is set to the current front
    if ( back == NULL)
        back = front;
    else
        current->prev = front;

}
/* clear by user : remove all node
 * delete the dynamic distributed space to avoid memory leak; O(N)
 */
void TileList::clear() {
    TileNode* current = front;// delete from front to the back successively
    while ( current != NULL ){
        front = front -> next;
        delete current;
        current = front;
    }
    // clear the front and back point
    front = NULL;
    back = NULL;
}
/* draw the node from the back to the front succesively; O(N)
 * const: not change the structure of the list
 */
void TileList::drawAll(GWindow& window) const {
    TileNode* current =  back;
    bool isEmpt = (front == NULL && back == NULL);
    //if list is not empty, then draw each node from the back to the front
    if ( !isEmpt )
    {
        while( current!= NULL){
           current->draw(window);
           current = current->prev;
        }
    }
}
/* helper function: find the tile node that is clicked by user; O(N)
 * const: not change the structure of the list
 */
TileNode* TileList::findNode(int x, int y) const{
    TileNode* result = NULL;// if not find, return null
    TileNode* current = front;//search from the front
    bool isFind = false;
    //if find the clicked node, return the point of the node directly
    while( current!= NULL && !isFind )
    {
         isFind = current->contains(x,y);
         if(!isFind)
            current = current->next;
         else
            result = current;
    }
    return result;
}
/* highlit: change the color of clicked node to yellow; O(N)
 * const: not change the structure of the list
 */
void TileList::highlight(int x, int y) const {
    TileNode* current = findNode( x, y );// find the point of the clicked node
    current->color = "yellow";
}
/* helper function: remove the clicked node, including 3 cases; O(1)
 * case 1: current node = front
 * case 2: current node = back
 * case 3: current node between front and back exclusively
 */
bool TileList::removeNode( TileNode* current){
   // TileNode* current = findNode( x, y );
    //if we do not have node to remove, then return false directly
    bool remove_success = false;
    if(current != NULL)
    {
        //case 1: current node = front
        if ( current == front )
        {
            front = current->next;
            if ( front != NULL )
                front->prev = NULL;
            else // after removing current node, list will be empty, and thus back need to be set to NULL
                back = NULL;
        }
        else{
            //case 2: current node = back
            if( current == back )
            {
                back = current->prev;
                back->next = NULL;
            }
            //case 3: current node between front and back exclusively
            else // connect the node before the current node and the node behind the current node
            {
                current->prev->next = current->next;
                current->next->prev = current->prev;
            }

        }
         //delete current clicked node, and remove successfully
         delete current;
         remove_success = true;
    }
    return remove_success;
}
/* move the clicked node to the back of the list; O(N)
 */
void TileList::lower(int x, int y) {
    TileNode* current = findNode( x, y);//get the clicked node
    removeNode(current );
    //if we can remove the clicked node in the list, then put it ont the back
    if( removeNode( current ) )
    addBack( current->x, current->y, current->width, current->height, current->color );
}
/* move the clicked node to the front of the list; O(N)
 */
void TileList::raise(int x, int y) {
    TileNode* current = findNode( x, y );
    if( removeNode( current ) )
    addFront( current->x, current->y, current->width, current->height, current->color );
}
/* remove the clicked node; O(N)
 */
void TileList::remove(int x, int y) {
    TileNode* current = findNode( x, y );
    removeNode(current );
}
/* remove the all node including clicked position; O(N)
 */
void TileList::removeAll(int x, int y) {
    TileNode* current = findNode( x, y );
    // remove the relative node succesively
    while( removeNode(current) ){
        current = findNode( x, y );//find next one
    }
}
