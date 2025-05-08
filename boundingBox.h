#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

#include "Image.h"
#include "pbmio.h"
#include<vector>
#include<string>
using namespace std;

#define ABS(x) (x<0)?-x:x

class boundingBox
{
    static bool isIntersecting(int a, int b, int c, int d)
    {    
        if((a>=c&&a<=d)||(b>=c&&b<=d)||(c>=a&&c<=b)||(d>=a&&d<=b))
            return true;
        return false;
    }
public:
    int minx, miny, maxx, maxy;

    boundingBox(){}
    boundingBox(int min_x, int min_y, int max_x, int max_y)
    {
        minx=min_x; maxx=max_x;
        miny=min_y; maxy=max_y;
    }

    void printBoundingBoxCoords(){
        cout<<"Upper Coordinate: ("<<minx<<","<<miny<<") and Lower Coordinate: ("<<maxx<<","<<maxy<<")"<<endl;
    }

    bool isInside(boundingBox &other)
    {
        if(this->minx >= other.minx && 
            this->miny >= other.miny && 
            this->maxx <= other.maxx && 
            this->maxy <= other.maxy)
            return true;

        return false;			
    }

    bool isIntersectingX(boundingBox &other)
    {
        return isIntersecting(this->minx, this->maxx, other.minx, other.maxx);
    }

    bool isIntersectingY(boundingBox &other)
    {
        return isIntersecting(this->miny, this->maxy, other.miny, other.maxy);
    }    

    int width(){return ABS(this->maxx-this->minx)+1;}
    int height(){return ABS(this->maxy-this->miny)+1;}

    static void drawBoundingBox(Image &bb_img, boundingBox &bb, unsigned char color = 0)
    {
        for(int j=bb.minx; j<=bb.maxx; j++)
        {    
            bb_img[bb.miny][j] = color;
            bb_img[bb.maxy][j] = color;
        }
        for(int j=bb.miny; j<=bb.maxy; j++)
        {    
            bb_img[j][bb.minx] = color;
            bb_img[j][bb.maxx] = color;
        }
    }

    static void drawBoundingBoxes(Image &bb_img, vector<boundingBox> &bbList, unsigned char color = 0)
    {
        //draw boundingbox
        for(int i=0;i<bbList.size();i++)
        {
            drawBoundingBox(bb_img, bbList[i], color);
        }
    } 
};

#endif