/* The cpp for N-grams is aimed at generating new text that
 * sounds like it came from the same author.
 *
 * Thanks to the code posted on the website of cs106b, according
 * to which, I reviewed some usage of collections.
 *
 * by Zixuan Zhou
 */

#include <iostream>
#include <fstream>
#include "console.h"
#include "filelib.h"
#include "map.h"
#include "simpio.h"
#include "stack.h"
#include "queue.h"
#include "vector.h"
#include "random.h"
using namespace std;
/* begin window for welcome
 */
void OpenWindow();
/* get the value of N
 */
int getValue();
/*get the number of word in random text
 */
int getNum(int Value);
/*building a map of N-grams from prefixes to suffixes, and wraps around.
 */
Map<Queue<string>, Vector<string> > BuildMap(int N, ifstream &infile);
/*generate random text from the map of N-grams
 */
void GenerateRandomText(Map<Queue<string>, Vector<string> > &wordMap,int wordNum);

int main()
{
    // TODO: Finish the program!
    ifstream infile;
    Map<Queue<string>, Vector<string> > wordMap;//map of N-grams
    int N;//value of N
    int wordNum = 1;//the number of word in random text

    OpenWindow();
    promptUserForFile(infile, "Input file name?");//open file. If fail, reprompt
    N = getValue();
    wordMap = BuildMap(N, infile);
    /* if user assigns 0 to the number of word in random text, then exit.
     * if chosen N is bigger than the number of word in the original text,
     * report an error, and exit.
     */
    while(wordNum!=0 && !wordMap.isEmpty())
    {
        wordNum = getNum(N);
        if(wordNum !=0)
        GenerateRandomText(wordMap,wordNum);
    }

    cout << "Exiting." << endl;
    return 0;
}

void OpenWindow()
{
    cout <<"Welcome to CS 106B/X Random Writer ('N-Grams')!"<<endl
         <<"This program generates random text based on a document."<<endl
         <<"Give me an input file and an 'N' value for groups"<<endl
         <<"of words, and I'll create random text for you."<<endl<<endl;
}

int getValue()
{
    int N = 0;
    while (N < 2) // if N < 2, then reprompt
    {
        N = getInteger("Value of N?");
        if (N < 2)
            cout << "N must be 2 or greater." <<endl;
    }

    return N;
}

int getNum(int value)
{
    int wordNum = 1;
    /* if the number of word in the random text is 0, then quit
     * if the number of word in the random text is smaller than N,
     * then reprompt.
     */
    while(wordNum < value & wordNum !=0)
    {
        wordNum = getInteger("# of random words to generate (0 to quit)?");
        if(wordNum < value & wordNum !=0)
            cout << "Must be at least "<<value<<" words"<<endl<<endl;
    }
    return wordNum;
}

Map<Queue<string>, Vector<string> > BuildMap(int N, ifstream &infile)
{
   Map<Queue<string>, Vector<string> > wordMap;
   Queue<string> window;//sliding window for N-1 string
   string tempWord;//to temporary word.
   Queue<string> beginWord;//to store N-1 string of the beginning for wrapsing around
   int numFileWord;//count the number of words in the orginal file.

   for (numFileWord = 0; infile >> tempWord; numFileWord++)
   {
       /* According to the prefix, put the new word in the map
        * for N-grams, then window slides
        */
       if ( window.size() == (N-1) )
       {
           wordMap[window] += tempWord;
           //window slides
           window.enqueue(tempWord);
           window.dequeue();
       }
       if (window.size() < (N-1))
       {
           beginWord.enqueue(tempWord);
           window.enqueue(tempWord);
       }
   }
    /*for wrapsing around, link the queue of beginning to the last.
     */
   while(!beginWord.isEmpty())
   {
       tempWord = beginWord.dequeue();
       wordMap[window] += tempWord;
       window.enqueue(tempWord);
       window.dequeue();
   }
   /* if N is larger than the number of words in the file, then quit.
    */
   if (numFileWord < N)
   {
       wordMap.clear();
       cout<<"N is larger than the number of words in the file."<<endl
           <<"Unable to compute N-grams."<<endl;
   }
   else
       cout<<endl;
   return  wordMap;
}

void GenerateRandomText(Map<Queue<string>, Vector<string> > &wordMap, int wordNum)
{
    Queue<string> window;//sliding window for N-1 string
    Queue<string> result;//to save the result for the random text.
    Vector<Queue<string> > mapKey;
    Vector<string> tempVec;//vector for temprorary value
    string tempWord;// string for temprorary value
    int next;//the index of vector for next string

    mapKey = wordMap.keys();//collect the keys of N-grams, for choosing beginning randomly
    int begin = randomInteger(0,mapKey.size()-1);
    window = mapKey[begin];
    result = window;
    //according to the map of N-grams, generate the random words for the new text.
    while(result.size() < wordNum)
    {
        tempVec = wordMap[window];
        //according to the key, randomly choose the index of elements in the vector,for next value.
        next = randomInteger(0,tempVec.size()-1);
        tempWord = tempVec[next];
        //window slides, put random string into the result
        result.enqueue(tempWord);
        window.enqueue(tempWord);
        window.dequeue();

    }

    //add the prefix and suffix, and output the result of the random text.
    cout<<"... ";
    while(!result.isEmpty())
    {
       cout<<result.dequeue()<<" ";
    }
    cout<<"..."<<endl<<endl;
}






