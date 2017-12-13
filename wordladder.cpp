/* This is the cpp for word ladder, which is a connection from
 * one word to another formed by changing one letter at a time
 * with the constraint that at each step the sequence of letters
 * still forms a valid word.
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
#include "lexicon.h"
#include "simpio.h"
#include "stack.h"
#include "queue.h"
#include "vector.h"

using namespace std;
/* begin window for welcome
 */
void OpenWindow();
/* output the ladder for two words
 */
void OutputWindow(Lexicon &dicWords);
/* transform capital letters into lowercase
 */
void transformLetter(string &word);
/* check the validity of input; if the word is illegal, then
 * reprompt.
 */
Vector<string> CheckWord(Lexicon &dicWords);
/*save the words from dictionary in a Lexicon
 */
void StoreDicWords(Lexicon &dicWords,ifstream &infile);
/* the funciton employs breath-first algorithm to find the ladder
 * between two words.
 */
Stack<string> FindLadder(Lexicon &dicWords, Vector<string> &wordPair);


int main()
{
    ifstream infile;
    Lexicon dicWords;//save the words from dictionary
    OpenWindow();
    promptUserForFile(infile, "Dictionary file name:");//open file. If fail, reprompt
    StoreDicWords(dicWords,infile);
    OutputWindow (dicWords);

    cout << "Have a nice day." << endl;
    return 0;
}

void OpenWindow()
{
    cout <<"Welcome to CS 106B/X Word Ladder!"<<endl
         <<"Give me two English words, and I will change the first"<<endl
         <<"into the second by changing one letter at a time."<<endl<<endl;
}

void OutputWindow(Lexicon &dicWords)
{

    bool bye = false; //whether it should quit.
    Vector<string> wordPair;//word1 and word2
    Stack<string> ladder;
    //if the input is empty, then quit
    while(!bye)
    {
        wordPair = CheckWord(dicWords);
        //if the ladder is empty, report the error, and if not, output the ladder.
        if(wordPair.isEmpty() == false)
        {
            ladder = FindLadder(dicWords,wordPair);
            if(!ladder.isEmpty())
                cout << "A ladder from "<<wordPair[1]
                     << " back to "<<wordPair[0]<<":"<<endl;
            else
                cout << "No word ladder found from " <<wordPair[1]
                     << " back to "<<wordPair[0]<<"."<<endl;
            while(!ladder.isEmpty())
                cout << ladder.pop() <<" ";

            cout << endl;
        }
        else
            bye = true;
    }

}

void StoreDicWords(Lexicon &dicWords,ifstream &infile)
{
    //save the word from dictionary in the lexicon
    string tempWord;//string for temporary usage
    while(infile >> tempWord) {
        dicWords.add(tempWord);
    }
}

void transformLetter(string &word)
{
    /*if receive the word contains the captital letter, then
     * transform into lowercase.
     */
    for(int i = 0;i < word.size(); i++)
        if( word[i]<='Z' && word[i]>='A' )
            word[i] += 32;

}

Vector<string> CheckWord(Lexicon &dicWords)
{
    string word1;
    string word2;
    Vector<string> wordPair;//save the word1 and word2, return the result
    bool reprompt = true;//whether illegal
    //if receive the illegal word, then report the error and reprompt
    while(reprompt)
    {
        cout <<endl;
        //if the input of word1 or word2 is empty, then quit
        getLine("Word 1 (or Enter to quit):", word1);
        if(word1.size()==0)
            return wordPair;
        getLine("Word 2 (or Enter to quit):", word2);
        if(word2.size()==0)
            return wordPair;
        //transform capital letter into lowercase
        transformLetter(word1);
        transformLetter(word2);

        if(word1.size() != word2.size())
            cout <<"The two words must be the same length."<<endl;
        else
        {
            if (dicWords.contains(word1) == false || dicWords.contains(word2) == false)
                cout <<"The two words must be found in the dictionary."<<endl;
            else
            {
                if(word1.compare((word2)) == 0)
                    cout <<"The two words must be different."<<endl;
                else
                {
                    reprompt = false;

                }
            }
        }
    }
    //save the word1 and word2 in the vector, then return
    wordPair.add(word1);
    wordPair.add(word2);
    return wordPair;
}

Stack<string> FindLadder(Lexicon &dicWords, Vector<string> &wordPair)
{
    Queue <Stack<string> > QueueStore;//save the obtained word structure
    bool FindLadder = false;//whether the ladder is found
    string word1 = wordPair[0];
    string word2 = wordPair[1];
    string tempWord;//string for temporary usage, save the changed word
    string tempWord1 = word1;//string for temporary usage, save the original word
    Stack <string> tempSt;//stack for temporary usage
    Lexicon monitor;//save the obtained word to avoid repeating

    monitor.add(word1);//add the original word1
    tempSt.push(tempWord1);
    QueueStore.enqueue(tempSt);

    while(FindLadder == false)
    {
        tempSt = QueueStore.dequeue();
        tempWord1 = tempSt.peek();
        /* change the letter of the word from 'a' to 'z' successively
         * to obtain the word which can be found in the dictionary.
         */
        for(int k=0;k<tempWord1.size() && FindLadder == false;k++)
        {
            tempWord = tempWord1;

            for(char j='a'; j<='z'&& FindLadder == false; j++)
            {
                tempWord[k] = j;
                /* if the current word can be found in the dictionary,
                 * and is not a repeated word, then put into our structure and
                 * monitor.
                 */
                if(dicWords.contains(tempWord) == true && monitor.contains(tempWord) == false)
                {
                        tempSt.push(tempWord);
                        QueueStore.enqueue(tempSt);
                        monitor.add(tempWord);
                        tempSt.pop();
                    //if find the word2, end the loop
                    if(tempWord.compare(word2) == 0)
                        FindLadder=true;
                }
            }
        }
        tempSt.clear();
        //if we cannot find the ladder when the queue is empty, then end the loop
        if(QueueStore.isEmpty())
            FindLadder = true;
    }
    //if we can find the ladder, then the last stack in this queue will be the result.
    if(!QueueStore.isEmpty())
        tempSt = QueueStore.back();

    return tempSt;
}


