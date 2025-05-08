#ifndef EDGE_DETECTION_H
#define EDGE_DETECTION_H
#include "Image.h"

Image edge(Image &image)
{
    int kernel[3][3] = {{1, 1, 1},
                        {1, -8, 1},
                        {1, 1, 1}};

    Image res(image.width, image.height, 255);

    for(int i=1; i<image.height-1; i++)
    {
        for(int j=1; j<image.width-1; j++)
        {
            int sum=0;
            for(int p=0; p<3; p++)
            {
                for(int q=0; q<3; q++)
                {
                    sum += kernel[p][q]*image[i+p-1][j+q-1];

                }
            }
            res[i][j] = (sum > 0)? 0: image.maxval;
        }
    }

    return res;
}

#endif