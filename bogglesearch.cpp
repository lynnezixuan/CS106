/* The CPP, using backtracking to search words on Boggle, which is a board game with a N*N square grid of
 * letter cubes where players try to make words by connecting letters on adjacent cubes.
 *
 * Thanks to the video on YEAH HOURS.
 *
 * by Zixuan Zhou
 */
#include <string>
#include "grid.h"
#include "lexicon.h"
#include "set.h"
#include "vector.h"
#include "shuffle.h"
#include "random.h"
#include "bogglegui.h"
using namespace std;
/* simulate rolling a boggle box : randomly choose their up sides and then rearrange letter cubes
 */
Grid<char> generateRandomBoard(int size, const Vector<std::string>& letterCubes) {

    Grid<char> randomBoard(size,size); // generated size by size random board
    // generate each side for letter cube, which is assigned to the orginal board
    for (int i = 0; i < size;i++)
    {
        for (int j = 0; j < size;j++)
        {
            //choose each letter from the string vector, and assign it on the board
            string str = letterCubes[i*size+j];
            int random_side = randomInteger(0,str.size()-1);//random side of the letter cube
            randomBoard[i][j] = str[random_side];
        }
    }
    //generate random board
    shuffle(randomBoard);
    return randomBoard;
}

/* according to the rule of scorig, counting the score for human and computer
 */
void countScore(Set<string> word_set, string score_type){
    int min_size = BoggleGUI::MIN_WORD_LENGTH;// min size of the valid word
    int score = 0;

    for(string str:word_set)
    {
        int length = str.size();
        if (length <= min_size)
            score += 1;
        else if (length == min_size+1)
            score += 2;
        else if(length == min_size+2)
            score += 3;
        else if(length == min_size+3)
            score += 5;
        else
            score += 11;
    }
    //count score for human and computer respectively
    if(score_type == "human")
         BoggleGUI::scorePointsHuman(score);
    else
         BoggleGUI::scorePointsComputer(score);

}

/* This function belongs to human word search part, aimed at fingding start position of human word
 * **********************************************************************************************
 * FindStart: After obtaining the first letter of human word, search it on the board,
 * and return their positions via vector.
 */
Vector<int> findStart(const Grid<char>& board,char start){
    Vector<int> start_pos;// save the start position
    int size = board.numCols();//size of boggle board
    for (int i = 0; i < size;i++)
    {
        for (int j = 0; j < size;j++)
        {
            BoggleGUI::setHighlighted(i,j); //highlighting animation
            if(start == board[i][j]) // if find the position, record it in the vector
            {
               start_pos.add(i*10+j);// convert a posion info into a 2-digit integer
            }
            else
            BoggleGUI::setHighlighted(i,j,false);//if it is not target position, then cancel the animation
        }

    }
    return start_pos;
}

/* This function belongs to human word search part, aimed at testing human input word
 * *********************************************************************************
 *  Recursion function, using backtracking, confirm whether the human input word can
 *  be found on the board
 */

bool humanWordSearchHelper(const Grid<char>& board, string word,Set<int> &seen_pos,int start_row,int start_col) {
    //base case, once obtaining the word on the board, return true immediately
    if(word.size()==0)
        return true;
    //recursive case, if not find the word, continue exploring
    else{
        //explore the next letter amoung the current cube's 8 neighbors
        for (int i = -1; i < 2;i++)
        {
            for (int j = -1; j < 2;j++){
                //current neigbor's position
                int current_row = start_row+i;
                int current_col = start_col+j;
                //only if the position of neigbor is valid on the board, continue the exploring.
                bool in_bound = board.inBounds(current_row,current_col);
                if(in_bound)
                {
                    int pos = 10*current_row+current_col;// convert a posion info into a 2-digit integer
                    //if haven't explored the position yet, then continue the exploring
                    if(!seen_pos.contains(pos))
                    {
                        char str = word[0];//the first letter of current word, which is the target letter for this searching
                        word = word.substr(1);
                        seen_pos.add(pos);//mark the explored position
                        BoggleGUI::setHighlighted(current_row,current_col);//hightlight the exploring point
                        if(str == board[current_row][current_col])//if find the target letter, then continue the recursion.
                        {
                            if(humanWordSearchHelper(board,word,seen_pos,current_row,current_col))
                                return true;//if find the human word on the board, return ture immediately
                        }
                        BoggleGUI::setHighlighted(current_row,current_col,false);
                        seen_pos.remove(pos);//undo the mark
                        word = str + word;//recover the word
                    }
                }
            }
        }
        return false;
    }
}

/* This function belongs to human word search part, aimed at preprocessing the human input to simpilify the recursion.
 * *******************************************************************************************************************
 * Pass start position : get start position from findStart function, pass the value to the recursion function, and test
 * human word from different start points.
 */
bool getHumanWordSearchValue(const Grid<char>& board, const string& word) {
    Set<int> seen_pos;//mark the explored position
    Vector<int> start;//save the possible start position, 2 - digit integer
    Set<string> word_set;//the set save human valid input for scoring
    start = findStart(board,word[0]);
    for(int i = 0; i<start.size();i ++)
    {
        // convert 2 - digit integer, into (x,y) start position,
        int start_col = start[i]%10;
        int start_row = (start[i]-start_col)/10;
        string word_human = word.substr(1);//left word(without first letter)
        seen_pos.add(start[i]);// mark the first letter
        BoggleGUI::setHighlighted(start_row,start_col);
        //if the recursion function finds the human word on the board, count the score and return true immediately
        if(humanWordSearchHelper(board,word_human,seen_pos,start_row,start_col))
        {
            word_set.add(word);
            countScore(word_set,"human");
            return true;
        }

    }
    return false;


}
/* This is the wrapper function belongs to human word search part
 * *************************************************************************************************
 * Test validation : test the length of human word, and confirm whether can be found in the dictionary
 */
bool humanWordSearch(const Grid<char>& board, const Lexicon& dictionary, const string& word) {

    bool non_valid = (word.size() < BoggleGUI::MIN_WORD_LENGTH)||(!dictionary.contains(word));
    BoggleGUI::clearHighlighting();

    if( non_valid )
        return false;
    else
        return getHumanWordSearchValue(board,word);
}

/* This function belongs to computer word search part, aimed at searching all valid word on the board
 * **************************************************************************************************
 *  Recursion function, using backtracking, finding all valid word on the board from different start
 *  points(N * N)
 */
void computerWordSearchHelper(const Grid<char>& board, const Lexicon& dic, Set<string> & word_set,
                              Set<int> &seen_pos,int start_row,int start_col,string build) {
    //base case: if cannot find the building string on the dictionary, then prune the twig
    if(!dic.containsPrefix(build))
        return;
    //recursive case: if can find the prefix in the dictionary, then continue exploring
    else{
        //if building string is valid, put int computer word set
        bool isValid_word = dic.contains(build)&&(build.size()>=BoggleGUI::MIN_WORD_LENGTH);
        if(isValid_word)
            word_set.add(build);
        //explore the next letter amoung the current cube's 8 neighbors
        for (int i = -1; i < 2;i++)
        {
            for (int j = -1; j < 2;j++)
            {
                //current neighbor's position
                int current_row = start_row+i;
                int current_col = start_col+j;
                //only if the position of neigbor is valid on the board, continue the exploring.
                bool in_bound = board.inBounds(current_row,current_col);
                if(in_bound)
                {
                    // convert a posion info into a 2-digit integer
                    int pos = 10*current_row+current_col;
                    //obtain the letter of current neighbor
                    char current_letter = board[current_row][current_col];
                    //if haven't explored the position yet, then continue the exploring
                    if(!seen_pos.contains(pos))
                    {
                        build = build + current_letter;//add the current letter to building string
                        seen_pos.add(pos);//mark the explored position
                        computerWordSearchHelper(board,dic,word_set,seen_pos,current_row,current_col,build);
                        seen_pos.remove(pos);//undo mark
                        build = build.substr(0,build.size()-1);//remove the current letter
                    }
                }
            }
        }
    }
}

/* This function belongs to computer word search part, aimed at preprocessing the task to simpilify the recursion.
 * *******************************************************************************************************************
 * Pass start position : get start position, and pass the value to the recursion function, and search all valid word on
 * board from different start points(N * N).
 */
Set<string> getComputerWordSearchValue(const Grid<char>& board, const Lexicon& dictionary,  const Set<string>& humanWords){
    Set<string> word_set;//valid word set for computer for scoring
    Set<int> seen_pos;//mark the explored position
    string start_letter;//first letter of building string ( which is the current building word )
    int size = board.numCols();

    for (int start_row = 0; start_row < size;start_row++)
    {
        for (int start_col = 0; start_col < size;start_col++)
        {
            seen_pos.add(start_row*10+start_col);//mark the start position
            start_letter += board[start_row][start_col];
            //recursion function
            computerWordSearchHelper(board,dictionary,word_set,seen_pos,start_row,start_col,start_letter);
            seen_pos.remove(start_row*10+start_col);//undo mark
            start_letter = start_letter.substr(1);//clear the first letter of building string
        }
    }
    word_set = word_set - word_set*humanWords;//remove words appeared in human set
    countScore(word_set,"computer");
    return word_set;
}
/* This is the wrapper function belongs to computer word search part
 * *******************************************************************
 */
Set<string> computerWordSearch(const Grid<char>& board, const Lexicon& dictionary, const Set<string>& humanWords) {

    return getComputerWordSearchValue(board,dictionary,humanWords);

}
