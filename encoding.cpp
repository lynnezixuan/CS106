/* This is the CPP file for Huffman, building a file compression algorithm
 * that uses binary trees and priority queues.
 *
 * by Zixuan Zhou
 */

#include "encoding.h"
#include "pqueue.h"
#include "filelib.h"
/* counting frequncies of character
 * arg: stream of input character
 * return: a frequncy map
 */
Map<int, int> buildFrequencyTable(istream& input) {
    Map<int, int> freqTable;
    int temp = input.get(); // current character
    while(  temp!= -1 ){ // if not reach the end of stream, continue
        freqTable[temp] += 1;
        temp = input.get();
    }
    freqTable[PSEUDO_EOF] = 1;//put eof into the freq map
    return freqTable;
}
/* build an encoding tree, with each node storing a character and
 * count of occurrence.
 * arg: freq map/table
 * return: pointer to the root of encoding tree
 */
HuffmanNode* buildEncodingTree(const Map<int, int>& freqTable) {
    PriorityQueue<HuffmanNode*> pq; // priority queue
    HuffmanNode* root;
    // according to the freq table, put nodes into priority queue
    for (int key:freqTable){
        HuffmanNode* ptr = new HuffmanNode;
        ptr->character = key;
        ptr->count = freqTable[key];
        pq.enqueue(ptr,freqTable[key]);
    }
    //build the encoding tree
    while ( pq.size() > 1) // only if there is one root node in the queue, building will stop.
    {
        //dequeue two nodes each time
        HuffmanNode* ptr1;
        HuffmanNode* ptr2;
        ptr1 = pq.dequeue();
        ptr2 = pq.dequeue();
        HuffmanNode* ptr = new HuffmanNode;
        // the created parent node of those two nodes is not a char
        ptr->character = NOT_A_CHAR;
        ptr->count = ptr1->count + ptr2->count;
        //two nodes become the left and right childern of the created parent node
        ptr->zero = ptr1;
        ptr->one = ptr2;
        pq.enqueue(ptr,ptr->count);
    }
    //the last node of priority queue is the root of the encoding tree
    root = pq.dequeue();
    return root;
}
/* helper function of building an encoding map
 * recurive function
 */
void buildEncodingMapHelper(Map<int, string> &encodingMap, HuffmanNode* current, string build){
    //base case: curret pointer points to null
    if( current == NULL)
        return;
    //recursive case: traversal of encoding tree,post-order
    else {
            int current_char = current->character;
            //if the current character is valid, put it into encoding map
            if( current_char != NOT_A_CHAR )
            encodingMap[current_char] = build;
            //encode each left branch as a bit value of 0 and each right branch as a bit value of 1
            buildEncodingMapHelper(encodingMap, current->zero, build + "0");
            buildEncodingMapHelper(encodingMap, current->one, build + "1");
    }
}
/* build an encoding map, derived from encoding tree
 * wrapper function
 * arg: encoding tree
 * return: encoding map
 */
Map<int, string> buildEncodingMap(HuffmanNode* encodingTree) {
    Map<int, string> encodingMap;
    buildEncodingMapHelper(encodingMap,encodingTree,"");
    return encodingMap;
}
/* according to the huffman code for current character, write binary code into
 * output stream
 */
void writeOutput(obitstream&output,string current_code){
    for(char temp: current_code){
        if (temp =='0')
            output.writeBit(0);
        else
            output.writeBit(1);
    }
}
/* encoding the text data into a shorter binary representation
 */
void encodeData(istream& input, const Map<int, string>& encodingMap, obitstream& output) {
    int temp = input.get();// current character
    string current_code;//the huffman code for current char
    while(  temp!= -1 ){//if not reach the file end, continue
        current_code = encodingMap[temp];
        writeOutput(output,current_code);//write the binary code into output
        temp = input.get();//move to next char
    }
   current_code = encodingMap[PSEUDO_EOF];//put the eof into the output
   writeOutput(output, current_code);
}
/* helper function of decoding file
 * recursive function: read each bit from the file, one at a time, and
 * use this bit to traverse the Huffman tree.
 */
int decodeDataHelper(ibitstream& input, HuffmanNode* node){
    //base case: reach the leaf of the encoding tree
    if( node->isLeaf() )
    {
        return node->character;
    }
    //recursive case: If the current bit is 0, move left in the tree. If the bit is 1, move right.
    else{
        int current = input.readBit();
        if ( current == 0 )
            return decodeDataHelper(input, node->zero);
        else
            return decodeDataHelper(input,  node->one);
    }

}
/* use a Huffman tree to decode a file
 * wrapper function
 */
void decodeData(ibitstream& input, HuffmanNode* encodingTree, ostream& output) {
    int current_char = 0;
    //decode a char each time/recursion
    while( current_char != PSEUDO_EOF)//if not reach the file end, continue
    {
        current_char = decodeDataHelper(input, encodingTree);
        if( current_char != PSEUDO_EOF )
            output.put(current_char);
    }

}
/* overall compression function: compress the given input file into the given output file.
 * read the input file one character at a time, building an encoding of its contents, and
 * write a compressed version of that input file, including a header, to the specified
 * output file.
 */
void compress(istream& input, obitstream& output) {
    //build frequency table
    Map<int, int> freqTable = buildFrequencyTable( input );
    //rewind istream for encoding function
    rewindStream(input);
    //compress freq map into output
    output << freqTable;
    //build encoding tree, according to freq map
    HuffmanNode*encodingTree = buildEncodingTree(freqTable);
    //build encoding map, according to encoding tree
    Map<int, string> encodingMap = buildEncodingMap(encodingTree);
    //encoding data, according to encoding map
    encodeData(input, encodingMap, output);
}
/* overall decompression function: read the bits from the given input file one at a time,
 * including header packed inside the start of the file, to write the original contents
 * of that file to the file specified by the output parameter.
 */
void decompress(ibitstream& input, ostream& output) {
    Map<int, int> freqTable;
    //get the freq table from compressed input
    input >> freqTable;
    // build encoding tree, according to the freq table
    HuffmanNode*encodingTree = buildEncodingTree(freqTable);
    //decode data, according to the encoding tree
    decodeData(input, encodingTree, output);
}
/* release dynamic distributed space of nodes in tree
 * recursive function
 * traversal of the tree
 */
void freeTree(HuffmanNode* node) {
    //base case: current pointer points to null, already reached leaf of the tree
    if( node == NULL)
        return;
    //recursive case:traversal of the tree, release the node one by one
    else{
        freeTree(node->one);
        freeTree(node->zero);
        delete node;
        node = NULL;
        }
}
