/* This is the cpp file for tilelist, window manager of your operating system.
 * The comments for each function are in the cpp file.
 * Refer to the code on the ppt from lecture15.
 *
 * by Zixuan Zhou
 */
#ifndef _tilelist_h
#define _tilelist_h

#include <string>
#include "gwindow.h"
#include "tilenode.h"
using namespace std;

class TileList {
public:
    TileList();
    ~TileList();
    void addFront(int x, int y, int width, int height, string color);
    void addBack(int x, int y, int width, int height, string color);
    void clear();
    void debug();
    void drawAll(GWindow& window) const;

    void highlight(int x, int y) const;
    void lower(int x, int y);
    void raise(int x, int y);
    void remove(int x, int y);
    void removeAll(int x, int y);

private:
    TileNode* front;// front point of the list
    TileNode* back;// back point of the list
    TileNode* findNode(int x, int y) const;
    bool removeNode(TileNode* current);
};

#endif // _tilelist_h
