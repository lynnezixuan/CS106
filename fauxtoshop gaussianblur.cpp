/*
 * This is the cpp for fauxtoshop, the photo editor with advanced functions,
 *including green screen, "scatter" blur effect, and edge detection. This
 *photo editor can also measure the difference between two pictures. Besides,
 *this cpp contains a hiden scatter function, employing gaussian blur method.
 *
 *Thanks to the video of the relative review session, this code could be bettered!
 *
 *by Zixuan Zhou
 *Oct.3.2017
 */

#include <iostream>
#include "math.h"
#include "string.h"
#include "stdlib.h"
#include "console.h"
#include "gbufferedimage.h"
#include "grid.h"
#include "gwindow.h"
#include "random.h"
#include "simpio.h"
#include "fauxtoshop-provided.h"   // instructor-provided code

using namespace std;
/*
 * This is the function, which opens the image, and show it on the
 * Gwindow.
 */
bool OpenImage(GWindow& gw,GBufferedImage& img);
/*
 * Thw menu for photo editor is shown.
 */
int ShowMenu();
/*
 * The function returns the user's option.
 */
void ChooseMenu(int OptionValue,Grid<int>& original,GBufferedImage& img, GWindow& gw);
/*
 * According to "radius", Scatter randomly select a pixel from
 * nearby in the original image for the current pixel in the new image.
 */
void Scatter(Grid<int>& original,GBufferedImage& img, GWindow& gw);
/*
 * According to "threshold", a pixel is defined by Edge Detection as an edge
 * if at least one of its neighbors has a difference of greater
 * than threshold from it.
 */
void EdgeDetection(Grid<int> & original,GBufferedImage& img,GWindow& gw);
/*
 * A "sticker" image is filtered by the Green Screen, which means any part
 * of the sticker that is close to pure green in color will be ignored, and
 * besides pasted on top of a "background" image.
 */
void GreenScreen(Grid<int> & original,GBufferedImage& img,GWindow& gw);
/*
 * Compare Image ask the user to name another image file, and will count
 * how many pixels differ between the two.
 */
void CompareImage(GBufferedImage& img,GWindow& gw);
/*
 * Different form the common Scatter, Scatter Gaussian employs gaussian
 * blur algorithm, which convolutes the original picture with the gaussian
 * kernel.
 */
void Scatter_Gaussian(Grid<int> & original,GBufferedImage& img,GWindow& gw);

int main()
{
        // basic setup of Graphics Window
        GWindow gw;
        gw.setTitle("Fauxtoshop");
        gw.setExitOnClose(true);
        gw.setVisible(true);
        GBufferedImage img;
        bool quit = false ;// whether quit from Fauxtoshop

        cout << "Welcome to Fauxtoshop!" << endl;

        while(!quit)
        {

        // ensure the same random numbers on each run
        fakeRandomNumberGenerator();
        // if filename is blank, then quit
        quit = OpenImage(gw,img);

        if(!quit)
        {
            // converting a BufferedImage to a Grid of ints
            Grid<int> original = img.toGrid();
            ChooseMenu(ShowMenu(),original,img, gw);
        }
        }

    cout << "Exiting." << endl;
    return 0;
}

bool OpenImage(GWindow& gw,GBufferedImage& img)
{
    //if image is not openned, then reprompt.
    bool isOpenFail=true;
    bool quit;
    while (isOpenFail)
    {
        string filename;
        getLine("Enter name of image file to open (or blank to quit):",filename);
        if(filename.size()==0)//blank to quit
        {
            quit = true;
            isOpenFail = false;
        }
        else
        {
            cout <<"Opening image file, may take a minute..." << endl;
            isOpenFail=!openImageFromFilename(img,filename);
            gw.setCanvasSize(img.getWidth(), img.getHeight());
            gw.add(&img, 0, 0);
        }
    }
    return quit;
}

int ShowMenu()
{
    int OptionValue = 0;

    while(OptionValue<1||OptionValue>4)
    {
        cout << "Which image filter would you like to apply?" << endl;
        cout << '\t' << "1 - Scatter" << endl
             << '\t' << "2 - Edge detection" << endl
             << '\t' << "3 - \" Green Screen \" with another image" << endl
             << '\t' << "4 - Compare image with another image" << endl;
        OptionValue = getInteger("Your choice:");
    }
    return OptionValue;
}

void ChooseMenu(int OptionValue,Grid<int>& original,GBufferedImage& img, GWindow& gw)
{
    switch (OptionValue)
    {
    case 1:
        //Scatter(original,img,gw);
        Scatter_Gaussian(original,img,gw);
        break;
    case 2:
        EdgeDetection(original,img,gw);
        break;
    case 3:
        GreenScreen(original,img,gw);
        break;
    case 4:
        CompareImage(img,gw);
        break;
    default:
        break;
    }

}

void Scatter(Grid<int> & original,GBufferedImage& img,GWindow& gw)
{
    int ScatterDegree = 0;//radius range of random number for scatter
    Grid<int> newPic;

    while(ScatterDegree<1||ScatterDegree>100 )
        ScatterDegree = getInteger("Enter  degree of scatter [1-100]:" );

    newPic.resize(original.numRows(),original.numCols());

    for(int i=0;i<original.numRows();i++)
    {
        for(int j=0;j<original.numCols();j++)
        {

            int row = -1;
            int col = -1;
            //if random (row+dr,col+dc) is out of the original bound, then do it again.
            while(!original.inBounds(row,col))
            {
                col = randomInteger(j-ScatterDegree,j+ScatterDegree);
                row = randomInteger(i-ScatterDegree,i+ScatterDegree);

            }
            newPic[i][j] = original[row][col];
    }

    }   
     //show the new image on the Gwindow
    img.fromGrid(newPic);
    gw.clear();
    gw.setCanvasSize(img.getWidth(), img.getHeight());
    gw.add(&img, 0, 0);

    //if input is not blank, then save the image.
    string newFilename;
    getLine("Enter filename to save image (or blank to skip saving):",newFilename);
    if(strlen(newFilename.c_str())!=0)
    {
        saveImageToFilename(img,newFilename);
        cout<<endl;
    }

}

void Scatter_Gaussian(Grid<int> & original,GBufferedImage& img,GWindow& gw)
{
    int ScatterDegree=0;  
    int row,col;
    int radius;
    int red, green, blue;
    double gaussRed,gaussGreen,gaussBlue;
    Vector<double> r;
    double gaussWeight;
    Grid<int> gaussPic;
    gaussPic.resize(original.numRows(),original.numCols());
    radius= ScatterDegree;
    gaussPic = original;

    while(ScatterDegree<1||ScatterDegree>100 )
       ScatterDegree = getInteger("Enter  degree of scatter [1-100]:");

    for(col=radius+1;col<original.numCols();col++)
    {
        for(row=radius+1;row<original.numRows();row++)
        {

            //divide the pixel into three color, which will be convoluted separately
            r=gaussKernelForRadius(radius);
            gaussRed=0;
            gaussGreen=0;
            gaussBlue=0;
            gaussWeight=0;

           // convolute the original picture with the gaussian kernel
            for(int j=0;j<2*radius+1&&(col-radius+j)<original.numCols();j++)
            {
                for(int i=0;i<2*radius+1&&(row-radius+i)<original.numRows();i++)
                {
                    GBufferedImage::getRedGreenBlue(original[row-radius+i][col-radius+j], red, green, blue);

                    gaussRed += red*r[i]*r[j];
                    gaussGreen += green*r[i]*r[j];
                    gaussBlue += blue*r[i]*r[j];
                    gaussWeight +=r[i]*r[j];

                }

            }
            gaussPic[row][col] = GBufferedImage::createRgbPixel(int(gaussRed/gaussWeight), int(gaussGreen/gaussWeight), int(gaussBlue/gaussWeight));

        }

    }
    //show the new image on the Gwindow
    img.fromGrid(gaussPic);
    gw.clear();
    gw.setCanvasSize(img.getWidth(), img.getHeight());
    gw.add(&img, 0, 0);

    //if input is not blank, then save the image.
    string newFilename;
    getLine("Enter filename to save image (or blank to skip saving):",newFilename);
    if(newFilename.size()!=0)
    {
        saveImageToFilename(img,newFilename);
        cout<<endl;
    }

}

void EdgeDetection(Grid<int> & original,GBufferedImage& img,GWindow& gw)
{
    int thershold=0;
    int red, green, blue;
    int colordis;
    int ajRed,ajGreen,ajBlue;
    bool isThershold;
    Grid<int> edgePic;

    while(thershold<=0)//therhold can only be positive, else reprompt
    thershold=getInteger("Enter thershold for edge detection:");

    edgePic.resize(original.numRows(),original.numCols());
    edgePic = original;

    for(int col=1;col<original.numCols();col++)
    {
        for(int row=1;row<original.numRows();row++)
        {

            GBufferedImage::getRedGreenBlue(original[row][col], red, green, blue);
            isThershold=false;
            int i=0;
                /*compare the current pixel with its nearbors, which compose a 3*3 matrix
                 *besides, while the algorithm on the ppt, which will go through
                 *all the matrix to find the max, this algo will return the judgement at once,
                 *when it find a value is bigger than thershold, due to which the efficiency
                 *will be improved a lot.
                 */
                for(int j=0;j<3&&original.inBounds((row+i-1),(col+j-1))&&!isThershold;j++)
                {
                    for( i=0;i<3&&original.inBounds((row+i-1),(col+j-1))&&!isThershold;i++)
                    {
                        GBufferedImage::getRedGreenBlue(original[row+i-1][col+j-1], ajRed, ajGreen, ajBlue);
                        colordis=max(max(abs(ajRed-red),abs(ajGreen-green)),abs(ajBlue-blue));

                        if(colordis>thershold)
                         isThershold=true;
                    }

                }
            //The pixels of edge will be black, and others will be white.
            if(isThershold)
                edgePic[row][col] = BLACK;
            else
                edgePic[row][col] = WHITE;
        }
    }
    //show the new image on the Gwindow
    img.fromGrid(edgePic);
    gw.clear();
    gw.setCanvasSize(img.getWidth(), img.getHeight());
    gw.add(&img, 0, 0);

    //if input is not blank, then save the image.
    string newFilename;
    getLine("Enter filename to save image (or blank to skip saving):",newFilename);
    if(newFilename.size()!=0)
    {
        saveImageToFilename(img,newFilename);
        cout<<endl;
    }

}

void GreenScreen(Grid<int> & original,GBufferedImage& img,GWindow& gw)
{ 
    GBufferedImage stic; 
    bool isOpenFail=true;
    string filename;

    cout<<"Now choose another file to add to your background image."<<endl;

    while (isOpenFail)
    {

        getLine("Enter name of image file to open:",filename);
        cout <<"Opening image file, may take a minute..." << endl;
        isOpenFail=!openImageFromFilename(stic,filename);
    }

    Grid<int> sticker = stic.toGrid();

    int thershold = 0;// tolerance therhold between 1 and 100
    int inputRow = 0;
    int inputCol = 0;
    string inpr,inpc;
    string location = "\n";
    bool isInputfail=true;//true = input invalid, false = valid input

    while(thershold < 1||thershold > 100)
        thershold = getInteger("Now Choose a tolerance threshold:");

    while(isInputfail)
    {
        getLine("Enter location to place image as \"(row,col)\"(or blank to use mouse):",location);
        //if the input is blank, ask user to click the pic with mouse.
        if(strlen(location.c_str())==0)
        {
            cout <<"Now click the background image to place new image:"<<endl;
            getMouseClickLocation(inputRow,inputCol);
            isInputfail = false;
            cout <<"You Chose ("<<inputRow<<","<<inputCol<<")"<<endl;
        }
        else
        {
            //find the (,), locate the row and col in it, and convert the string to int.
            if(location.find("(")!=std::string::npos&&location.find(",")!=std::string::npos&&location.find(")")!=std::string::npos)
            {
                inpr = location.substr(1,location.find(",")-location.find("(")-1);
                inpc = location.substr(location.find(",")+1,location.find(")")-location.find(",")-1);
                inputRow = atoi(inpr.c_str());
                inputCol = atoi(inpc.c_str());

                if(original.inBounds(inputRow,inputCol))
                    isInputfail =false;
            }
        }
    }       

    int row,col;
    int red, green, blue;
    int colordis;
    int stickerCol,stickerRow;
    Grid<int> greenPic = img.toGrid();

    col=inputCol;
    row=inputRow;

    //if the pixel of the sticker is green, it will not change the pixel of the background.
    for(stickerCol=0;stickerCol<sticker.numCols()&&(stickerCol+col)<greenPic.numCols();stickerCol++)
    {

        for(stickerRow=0;stickerRow<sticker.numRows()&&(stickerRow+row)<greenPic.numRows();stickerRow++)
        {

            GBufferedImage::getRedGreenBlue(sticker[stickerRow][stickerCol], red, green, blue);
            //compare the pixel with the pure green
            colordis=sqrt(pow(abs(red),2)+pow(abs(green-255),2)+pow(abs(blue),2));
            if(colordis>thershold)
                greenPic[stickerRow+row][stickerCol+col]=sticker[stickerRow][stickerCol];
        }
    }

    //show the new picture on the Gwindow
    img.fromGrid(greenPic);
    gw.clear();
    gw.setCanvasSize(img.getWidth(), img.getHeight());
    gw.add(&img, 0, 0);

    //if input is not blank, save the image.
    string newFilename;
    getLine("Enter filename to save image (or blank to skip saving):",newFilename);
    if(strlen(newFilename.c_str())!=0)
    {
        saveImageToFilename(img,newFilename);
        cout<<endl;
    }

}

void CompareImage(GBufferedImage& img,GWindow& gw)
{
    GBufferedImage otherImg;
    bool isOpenFail=true;
    int diffImage;
    cout<<"Now choose another image file to compare to."<<endl;

    //if the image is not openned successfully, then reprompt.
    while (isOpenFail)
    {
        string filename;
        getLine("Enter name of image file to open:",filename);
        isOpenFail=!openImageFromFilename(otherImg,filename);
        cout <<"Opening image file, may take a minute..." << endl;
    }

    //the function for comparing difference between two pics is provided.
    diffImage=img.GBufferedImage::countDiffPixels(otherImg);

    if(diffImage==0)
        cout<< "These images are the same!"<<endl<<endl;
    else
    {
        cout <<"These images differ in " << diffImage <<" pixel locations!"<<endl;
        showDiffWindow(gw, otherImg.getFilename());
        cout<<endl;
    }

}


