#ifndef RLSA_H
#define RLSA_H

#include "Image.h"
#include<iostream>
using namespace std;

void hrlsa(Image &image, int hgap)
{
    int previousBlack, k;

    // horizontal
    for(int i=0; i<image.height; i++)
    {
        previousBlack = -1;
        for(int j=0; j<image.width; j++)
        {
            if(image[i][j] == (unsigned char)0)            
            {
                if((j - previousBlack) < hgap)
                {
                    k = previousBlack+1;
                    while(k<j)
                    {
                        image[i][k]=(unsigned char)0;
                        k++;
                    }
                }
                previousBlack = j;
            }
        }
    }

}
void vrlsa(Image &image, int vgap)
{
    int previousBlack, k;

    //vertical
    for(int j=0; j<image.width; j++)
    {
        previousBlack = -1;
        for(int i=0; i<image.height; i++)
        {
            if(image[i][j] == (unsigned char)0)            
            {
                if((i - previousBlack) < vgap)
                {
                    k = previousBlack+1;
                    while(k<i)
                    {
                        image[k][j]=(unsigned char)0;
                        k++;
                    }
                }
                previousBlack = i;
            }
        }
    }
}
void rlsa(Image &image, int hgap, int vgap)
{
    int previousBlack, k;

    hrlsa(image, hgap);
    vrlsa(image, vgap);
}

#endif