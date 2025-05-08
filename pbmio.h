#ifndef PBM_IO
#define PBM_IO

#include "Image.h"
#include<fstream>
#include<string>
using namespace std;

void write_pbm(string filename, Image &image)
{
    int height = image.height, width = image.width, maxval=image.maxval;
    ofstream fout(filename, ios::binary);
    if(!fout.is_open())
    {
        throw "Error! Unable to open file: "+filename;
    }

    fout << "P4\n" << width << " " << height << "\n";

    char ch;
    for(int i=0;i<height;i++)
    {
        int shiftedbit=128;
        unsigned char byte=0;
        for(int j=0;j<width;j++)
        {
            if(image[i][j]==0)
                byte |= shiftedbit;
            shiftedbit >>= 1;
            if(shiftedbit==0)
            {
                ch = (char)byte;
                fout.write(&ch,1);
                shiftedbit=128;
                byte=0;
            }
        }
        if(shiftedbit!=128)
        {
            ch = (char)byte;
            fout.write(&ch,1);
        }

    }
    fout.close();
}

Image read_pbm(string filename)
{
    ifstream fin(filename, ios::binary);
    if(!fin.is_open())
    {
        throw "Error! Unable to open file: "+filename;
    }

    string buffer;

    getline(fin, buffer);
    if(buffer != "P4")
    {
        throw "Error! Unknown file format! A PBM P4 file is required";
    }

    getline(fin, buffer);
    while(buffer[0]=='#')
        getline(fin, buffer);

    int width, height, maxval=255;
    sscanf(buffer.c_str(), "%d %d\n", &width, &height);
    
    Image image(width, height);

    char ch;
    for(int i=0;i<height;i++)
    {
        int shiftedbit=128;
        fin.read(&ch, 1);
        unsigned char byte=(unsigned char)ch;

        for(int j=0;j<width;j++)
        {
            if(byte&shiftedbit)
                image[i][j]=(unsigned char)0;
            else
                image[i][j]=(unsigned char)maxval;

            shiftedbit >>= 1;
            if(shiftedbit==0 && j<width-1)
            {
                fin.read(&ch, 1);
                byte=(unsigned char)ch;
                shiftedbit=128;
            }
        }
    }
    fin.close();

    return image;
}

#endif
