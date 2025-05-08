#ifndef ISAUVOLA_H
#define ISAUVOLA_H

#include "Image.h"
#include "otsu.h"
#include "sauvola.h"

#include<cmath>
#include<stack>
#include<utility>
using namespace std;

Image contrastImg(Image &image)
{
    int height = image.height, width = image.width, maxval=image.maxval;

    Image contrastimg(width, height);
    Image temp(width, height);
    Image minmat(width, height);
    Image maxmat(width, height);

	//minmat
    for(int i=0; i<height; i++)
    {
        for(int j=1; j<width-1; j++)
        {
            temp[i][j] = min(min(image[i][j-1], image[i][j]), image[i][j+1]);
        }
        temp[i][0] = min(image[i][0], image[i][1]);
        temp[i][width-1] = min(image[i][width-1], image[i][width-2]);
    }

    for(int j=0; j<width; j++)
    {
        for(int i=1; i<height-1; i++)
        {
            minmat[i][j] = min(min(temp[i-1][j], temp[i][j]), temp[i+1][j]);
        }
        minmat[0][j] = min(temp[0][j], temp[1][j]);
        minmat[height-1][j] = min(temp[height-1][j], temp[height-2][j]);
    }
    //maxmat
    for(int i=0; i<height; i++)
    {
        for(int j=1; j<width-1; j++)
        {
            temp[i][j] = max(max(image[i][j-1], image[i][j]), image[i][j+1]);
        }
        temp[i][0] = max(image[i][0], image[i][1]);
        temp[i][width-1] = max(image[i][width-1], image[i][width-2]);
    }

    for(int j=0; j<width; j++)
    {
        for(int i=1; i<height-1; i++)
        {
            maxmat[i][j] = max(max(temp[i-1][j], temp[i][j]), temp[i+1][j]);
        }
        maxmat[0][j] = max(temp[0][j], temp[1][j]);
        maxmat[height-1][j] = max(temp[height-1][j], temp[height-2][j]);
    }

    double factor;
    for(int i=0; i<height; i++)
    {
        for(int j=0; j<width; j++)
        {
            factor = (double)(maxmat[i][j]-minmat[i][j])/(maxmat[i][j]+minmat[i][j]+0.0001);
            contrastimg[i][j] = (unsigned char)((int)(255*factor));
        }
    }

    otsu_thresh(contrastimg);
    return contrastimg;
}
int drawConnected(int i, int j, Image &image, Image &res)
{
    int height = image.height, width = image.width, maxval=image.maxval;

    int minh=i, maxh=i;

    stack<pair<int, int>> s;
    s.push(make_pair(i,j));

    while(!s.empty())
    {
        pair<int, int> index = s.top(); s.pop();
        int y=index.first, x = index.second;
        if(res[y][x]==maxval && image[y][x]==0)
        {
            res[y][x]=(unsigned char)0;

            if(y>maxh)
                maxh=y;
            else if(y<minh)
                minh=y;

            for(int p=y-1;p<=y+1;p++)
            {
                for(int q=x-1;q<=x+1;q++)
                {
                    if(!(p==y && q==x) && p>=0 && p<height && q>=0 && q<width)
                        s.push(make_pair(p,q));
                }
            }
        }
    }

    return maxh-minh+1;
}
int isauvola(Image &image)
{
    int height = image.height, width = image.width, maxval=image.maxval;

    Image contrastimg = contrastImg(image);
    sauvola(image, 0.02, 15);

    //skipped: morphological opening operator applied here in original paper

    vector<int> heights;
    long long sum=0, count=0;

    Image res(width, height, 255);
    for(int i=0; i<height; i++)
    {
        for(int j=0; j<width; j++)
        {
            if(res[i][j]==maxval && contrastimg[i][j]==maxval && image[i][j]==0)
            {
                int h=drawConnected(i,j,image, res);
                sum+=h;count++;
                heights.push_back(h);
            }
        }
    }

    image=res;

    sort(heights.begin(), heights.end());
    int mean = sum/count, median = heights[heights.size()/2];
    //cout << "iAvgH = " << mean << "\n";
    //cout << "iMedianH = " << median << "\n";
    
    return 3*median - 2*mean; //estimated mode
}

#endif
