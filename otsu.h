#ifndef OTSU_H
#define OTSU_H

#include "Image.h"
#include "threshold.h"

void histogram(double hist[], Image &image)
{
    int height = image.height, width = image.width, maxval=image.maxval;
    for(int i=0; i<maxval+1; i++)
        hist[i]=0;

    for(int i=0;i<height;i++)
    {
        for(int j=0;j<width;j++)
        {
            hist[(int)image[i][j]] += 1;
        }
    }

    double pixelnums = (width*height);
    for(int i=0; i<maxval+1; i++)
    {
        hist[i]/=pixelnums;
    }

}

void otsu_thresh(Image &image)
{
    int height = image.height, width = image.width, maxval=image.maxval;
    double hist[maxval+1];

    histogram(hist, image);

    int thresh;
    double p1=0, p2, sum1=0, sum2=0, v=0, u, maxv=-1;
    for(int t=0; t<=maxval; t++)
        sum2 += t*hist[t];

    for(int t=0; t<=maxval; t++)
    {
        p1 += hist[t];
        p2 = 1-p1;

        sum1 += t*hist[t];
        sum2 -= t*hist[t];

        u = (sum1/p1)-(sum2/p2);
        v = p1*(1.0-p1)*(u*u);
        if(v>maxv)
        {
            maxv=v;
            thresh=t;
        }
    }

    //cout << "Threshold = " << thresh << "\n";
    threshold(thresh, image);
}

#endif
