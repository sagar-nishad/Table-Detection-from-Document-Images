#ifndef THRESHOLD_H
#define THRESHOLD_H

#include "Image.h"

void threshold(int thresh, Image &image)
{
    int height = image.height, width = image.width, maxval=image.maxval;
    for(int i=0;i<height;i++)
    {
        for(int j=0;j<width; j++)
        {
            if((int)image[i][j] > thresh)
                image[i][j] = (unsigned char)maxval;
            else
                image[i][j] = (unsigned char)0;
        }
    }
}

#endif
