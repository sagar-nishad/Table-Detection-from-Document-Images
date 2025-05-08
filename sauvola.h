#ifndef SAUVOLA_H
#define SAUVOLA_H

#include "Image.h"

#include<cmath>
#include<vector>
using namespace std;

template<typename T>
vector<vector<int>> sumx(T &img, int window, int width, int height)
{
    vector<vector<int>> mat(height, vector<int>(width));
    int sum,a,b,halfwindow = window/2;
    for(int i=0; i<height; i++)
    {
        sum=0;
        for(int j=-halfwindow-1; j<halfwindow; j++)
            sum += img[i][(int)abs(j)];

        for(int j=0; j<width; j++)
        {
            b = j+halfwindow;
            a = j-halfwindow-1;

            a = (a<0)?-a:a;
            b = (b>=width)? 2*width-2-b: b;

            sum-=img[i][a];
            sum+=img[i][b];

            mat[i][j] = sum;
        }
    }
    return mat;
}

template<typename T>
vector<vector<int>> sumy(T &img, int window, int width, int height)
{
    vector<vector<int>> mat(height, vector<int>(width));
    int sum,a,b,halfwindow = window/2;

    for(int j=0; j<width; j++)
    {
        sum=0;
        for(int i=-halfwindow-1; i<halfwindow; i++)
            sum += img[(int)abs(i)][j];

        for(int i=0; i<height; i++)
        {
            b = i+halfwindow;
            a = i-halfwindow-1;

            a = (a<0)?-a:a;
            b = (b>=height)? 2*height-2-b: b;

            sum-=img[a][j];
            sum+=img[b][j];

            mat[i][j] = sum;
        }
    }
    return mat;
}
void sauvola(Image &image, double k=0.34, int w=15)
{
    int height = image.height, width = image.width, maxval=image.maxval;

    int R=128, wsquare=w*w, thresh;
    double s, m;// s=std deviation, m=mean

    vector<vector<int>> mat1 = sumx<Image>(image, w, width, height);
    vector<vector<int>> sum_mat = sumy<vector<vector<int>>>(mat1, w, width, height);

    for(int i=0;i<height;i++)
    {
        for(int j=0; j<width; j++)
        {
            mat1[i][j] = image[i][j]*image[i][j];
        }
    }


    vector<vector<int>> mat2 = sumx<vector<vector<int>>>(mat1, w, width, height);
    vector<vector<int>> sqr_sum_mat = sumy<vector<vector<int>>>(mat2, w, width, height);

    for(int i=0;i<height;i++)
    {
        for(int j=0; j<width; j++)
        {
            m = sum_mat[i][j]/wsquare;
            s = sqrt(((double)sqr_sum_mat[i][j]/wsquare) - m*m);
            thresh = (int)(m*(1+k*(s/R - 1)));
            image[i][j] = (unsigned char)((image[i][j]<thresh)?0:maxval);
        }
    }
}

#endif
