/* Fractals cpp for Sierpinski Triangle and Flood Fill.
 * Sierpinski Triangle: draws the Sierpinski triangle fractal image.
 * Flood Fill: write a recursive function to fill a region with a solid color.
 *
 * Thanks to the video of YEAH HOURS.
 *
 * by Zixuan Zhou
 */

#include "fractals.h"
#include "math.h"
using namespace std;
static const double SCALE_FACTOR = 0.5;//scale factor for Sierpinski Triangle
/* draw a triangle, with top/left position (x,y), and size of side.
 */
void drawTriangleHelper(GWindow& window, double x, double y, double size){
    window.drawLine(x,y,x+size,y);
    window.drawLine(x,y,x+(size/2),y+(sqrt(3)/2)*size);
    window.drawLine(x+(size/2),y+(sqrt(3)/2)*size,x+size,y);
}
/* recursion of drawSierpinskiTriangleHelper is actually a recursion function.
 * base case: order = 1, draw a triangle
 * recursive case: find the position for drawing base case triangle
 * Since we avoid re-drawing, only base case triangle will be drawn.
 */
void drawSierpinskiTriangleHelper(GWindow& window, double x, double y, double size, int order) {
    if (order==1)
    {
        drawTriangleHelper(window,x,y,size);
        return;
    }
    else
        drawSierpinskiTriangleHelper(window,x,y,size*SCALE_FACTOR,order-1);
        drawSierpinskiTriangleHelper(window,x+size*SCALE_FACTOR,y,size*SCALE_FACTOR,order-1);
        drawSierpinskiTriangleHelper(window,x+size*SCALE_FACTOR/2 ,y+(sqrt(3)/2)*size*SCALE_FACTOR,size*SCALE_FACTOR,order-1);
}
//wrapper function
void drawSierpinskiTriangle(GWindow& window, double x, double y, double size, int order){
    bool isBound;//if the (x , y) is not valid, then throw an error string
    isBound = window.inCanvasBounds(x,y);
    if (!isBound)
    {
        throw "exception";
        return ;
    }
    else
    drawSierpinskiTriangleHelper(window,x,y,size,order);
}
/* this helper funtion is actually the recursion function.
 * base case : if encounter the bound(wall), then stop
 * recursive case: change the original color with new color, and
 * find next pixel that could be altered.
 */

void floodFillHelper(GWindow& window, int x, int y,int originalColor,int color,int &difPixel){
    int currentColor;//color of current pixel(x,y)
    currentColor = window.getPixel(x,y);
    bool isWall;// if current pixel is out of bound, or out of the block, then stop.
    isWall = currentColor != originalColor||!window.inCanvasBounds(x,y);
    if (isWall)
        return;
    else
    {
        window.setPixel(x,y,color);
        difPixel += 1;//count the different pixel
        floodFillHelper(window, x+1, y,originalColor,color,difPixel);
        floodFillHelper(window, x, y+1,originalColor,color,difPixel);
        floodFillHelper(window, x-1, y,originalColor,color,difPixel);
        floodFillHelper(window, x, y-1,originalColor,color,difPixel);
    }
}
//wrapper function
int floodFill(GWindow& window, int x, int y, int color) {

    int originalColor;//record the color of original pixel
    int difPixel;//count the different pixel
    bool isBound;//if the (x , y) is not valid, then throw an error string
    originalColor = window.getPixel(x,y);
    isBound = window.inCanvasBounds(x,y);
    if (!isBound)
    {
        throw "exception";
        return 0;
    }
    if ( originalColor == color)// if the new color is same as the original one, then maintain
        return 0;
    else
        floodFillHelper(window, x, y,originalColor,color,difPixel);
        return difPixel;
}
