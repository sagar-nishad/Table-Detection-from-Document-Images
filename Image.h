#ifndef IMAGE_H
#define IMAGE_H

#include<cstring>
#include<string>
#include<algorithm>
using namespace std;

string image_filename = ""; // variable to store image filename which is used to save intermediate images


class Image
{
public:
    unsigned char **a;
    int width, height, maxval;

    Image(){a=nullptr; width=0; height=0; maxval=255;}
    
    Image(size_t w, size_t h)
    {
        width = w;
        height = h;
        maxval=255;
        
        a = new unsigned char*[height];
        for(int i=0; i<height; i++)
        {
            a[i] = new unsigned char[width];
        }
    }
    Image(size_t w, size_t h, int k)
    {
        width = w;
        height = h;
        maxval=255;
        a = new unsigned char*[height];
        for(int i=0; i<height; i++)
        {
            a[i] = new unsigned char[width];
            memset(&a[i][0], k, width);
        }
    }
    ~Image()
    {
        if(a!=nullptr)
        {
            for(int i=0; i<height; i++)
            {
                delete[] a[i];
            }
            delete[] a;
            a=nullptr;
        }
    }

    void copyInto(Image &other)
    {
        if(width!=other.width || height!=other.height)
            throw "Unable to copy into Matrix of unequal size!";

        for(int i=0; i<height; i++)
        {
            copy(&a[i][0], &a[i][0]+width, &other[i][0]);
        }
    }

    Image getCopy()
    {
        Image cpy(this->width, this->height);
        this->copyInto(cpy);
        return cpy;
    }

    unsigned char* &operator [](size_t i)
    {
    	return a[i];
    }
    
    Image& operator =(Image &other)   
    {   
        this->~Image();
        a = other.a;
        other.a = nullptr;
        return *this;
    }
    
    bool operator ==(Image &other)
    {
        if(width==other.width && height==other.height)
        {
            for(int i=0; i<height; i++)
            {
                if(memcmp(&a[i][0], &other.a[i][0], width)!=0)
                    return false;
            }
        }
        else
            throw "Unable to compare Matrix of unequal sizes!";

        return true;
    }

    bool operator !=(Image &other)
    {
        return !((*this)==other);
    }
};

#endif
