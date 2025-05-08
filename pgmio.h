#ifndef PGM_IO
#define PGM_IO

#include "Image.h"
#include<fstream>
#include<cstring>
#include<string>
using namespace std;

Image read_pgm(string filename)
{
    ifstream fin(filename, ios::binary);
    if(!fin.is_open())
    {
        throw "Error! Unable to open file: "+filename;
    }

    string buffer;

    getline(fin, buffer);
    if(buffer != "P5")
    {
        throw "Error! Unknown file format! A PGM P5 file is required";
    }

    getline(fin, buffer);
    while(buffer[0]=='#')
        getline(fin, buffer);

    int width, height, maxval;
    sscanf(buffer.c_str(), "%d %d\n", &width, &height);


    fin >> maxval;
    fin.get();

    Image image(width, height);

    char ch;
    for(int i=0;i<height;i++)
    {
        fin.read(reinterpret_cast<char*>(image[i]), width);
    }
    fin.close();
    return image;
}

void write_pgm(string filename, Image &image)
{
    int height = image.height, width = image.width, maxval=image.maxval;
    ofstream fout(filename, ios::binary);
    if(!fout.is_open())
    {
        throw "Error! Unable to open file: "+filename;
    }

    fout << "P5\n" << width << " " << height << "\n" << maxval << "\n";

    char ch;
    for(int i=0;i<height;i++)
    {
        fout.write(reinterpret_cast<char*>(image[i]), width);
    }
    fout.close();
}

#endif
