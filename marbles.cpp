/* This is the CPP for Mable game, which employs "backtracking" to write
 * a solver for the game of Marble Solitaire.
 *
 * Thanks to the video from YEAH HOURS.
 *
 * By Zixuan Zhou
 */

#include "grid.h"
#include "vector.h"
#include "marbletypes.h"
#include "marbleutil.h"
#include "compression.h"
#include "set.h"
using namespace std;
/* helper function for mable game, is a recursion function.
 * return true, if puzzle could be solved.
 */
bool solvePuzzleHelper(Grid<Marble>& board, int marbleCount, Vector<Move>& moveHistory,Set<BoardEncoding> &seenBoard) {
//base case
if(marbleCount == 1)
    return true;
//recursive case
else{
    //find all possible movement for this states of board
    Vector<Move> possibleMove = findPossibleMoves(board);
    //try all move
    for(int i = 0; i < possibleMove.size() ; i++){
        //record the movement
        moveHistory.add(possibleMove[i]);
        makeMove(possibleMove[i],board);
        //if we have seen this state of board, then withdraw it, otherwise continue.
        BoardEncoding current_board = compressBoard(board);
        if (!seenBoard.contains(current_board))
        {
            seenBoard.add(current_board);
            //if find the solution, stop and report immediately.
            if(solvePuzzleHelper(board,marbleCount-1,moveHistory,seenBoard))
                return true;
        }
        //withdraw the movement
        undoMove(possibleMove[i],board);
        moveHistory.remove(moveHistory.size()-1);
    }

    return false;
}

}
// wrapper function of solve marble game
bool solvePuzzle(Grid<Marble>& board, int marbleCount, Vector<Move>& moveHistory) {
    Set<BoardEncoding> seenBoard;// backtracking the seen states of board
    return solvePuzzleHelper(board,marbleCount,moveHistory,seenBoard);
}
