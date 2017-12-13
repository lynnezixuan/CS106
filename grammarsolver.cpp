/* This cpp for grammar solver accepts a reference
 * to an input file representing a language grammar,
 * along with a symbol to randomly generate a new
 * expansion.
 *
 * Thanks to the video of YEAH HOURS.
 *
 * by Zixuan Zhou
 */
#include "grammarsolver.h"
#include "map.h"
#include "strlib.h"
#include "filelib.h"
#include "random.h"

using namespace std;
/* read a file of BNF, store the grammar structure into a map,
 * which will be used as a reference to generate new expansion.
 */
void ReadFile(istream &infile,  Map <string,Vector<string> > &grammarMap){
    Vector <string> line;//read the file by line
    Vector <string> tempVec;// vector for temporary usage,save non-terminal(index = 0) and rules(index =1)
    Vector <string> rule;//a vector save the rules for non- terminal
    string temp;// string for temporary usage, save string of rules
    bool isValid;
    readEntireFile(infile, line);//save all line in a vector
    for(int j = 0; j < line.size();j++)
    {
        temp = line[j];
        tempVec = stringSplit(temp, "::=");
        temp = tempVec[1];
        rule = stringSplit(temp, "|");
        //tempVec[0] is non-terminal, if non-terminal has different rule, it is invalid, and throw a error string
        isValid = (grammarMap[tempVec[0]] == rule || grammarMap[tempVec[0]].isEmpty());
        if (isValid)
            grammarMap[tempVec[0]] = rule;
        else
            throw "exception";
    }

}
/* recursion function, according to grammar map, generates random expansion
 */
void grammarGenerateHelper(string symbol, Map <string,Vector<string> > &grammarMap, string &expansion) {
    bool isTerminal;
    Vector <string> rule;//a vector save the rules for non- terminal
    Vector <string> tempVec;// vector for temporary usage,save rules
    int ruleIndex;//randomly choose a rule for non-terminal
    /* string symbol may contain several rules.
     * e.g."<np> <vp>" includes "<np>" and "<vp>"
     */
    rule = stringSplit(symbol, " ");
    for (int i = 0; i < rule.size() ; i ++)
    {
        //base case: terminal can not be found in the grammar map
        isTerminal = !grammarMap.containsKey(rule[i]);
        if (isTerminal)
        {
            expansion.append(rule[i]);
            expansion.append(" ");

        }
        else//recursive case : randomly choose a rule of the non-terminal
        {
            tempVec = grammarMap[rule[i]];
            ruleIndex = randomInteger(0,tempVec.size()-1);
            symbol = tempVec[ruleIndex];
            grammarGenerateHelper(symbol,grammarMap,expansion);
        }
    }

}
// function calls the readfile funcition and generate grammar helper function
Vector<string> grammarGenerate(istream& input, string symbol, int times) {

    Map <string,Vector<string> > grammarMap;//a map which saves the structure of grammar
    Vector<string> v;//save random expansions
    string expansion;
    ReadFile(input,grammarMap);
    //loop for certain number(times) random expansion, according to the requirement from user
    for (int i = 0; i < times ; i++)
    {
         grammarGenerateHelper(symbol,grammarMap,expansion);
         //remove the redundant blank " "
         expansion = trim (expansion);
         v.add(expansion);
         expansion.clear();
     }

    return v;
}
