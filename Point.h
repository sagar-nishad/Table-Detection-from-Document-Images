#ifndef POINT_H
#define POINT_H

#include "Image.h"
#include<stack>
#include<utility>

struct Point
{
    int x,y;

    bool operator<(Point& other)
    { 
        if(this->y == other.y) return this->x < other.x;
        return this->y < other.y;
    }
    bool operator==(Point& other)
    {
        return this->x==other.x && this->y==other.y;
    }
};

Point newPoint(int X, int Y){Point p; p.x = X, p.y = Y; return p;}

void eightWayDraw(Image &image, Point &center, int x, int y)
{
    image[center.y+y][center.x+x] = 0;
    image[center.y-y][center.x-x] = 0;
    image[center.y+x][center.x+y] = 0;
    image[center.y-x][center.x-y] = 0;
    image[center.y+y][center.x-x] = 0;
    image[center.y-y][center.x+x] = 0;
    image[center.y+x][center.x-y] = 0;
    image[center.y-x][center.x+y] = 0;
}
void midPointCircle(Image &image, Point &point, int r=5)
{
    int p = 1 - r;
    int x = 0, y = r;

    eightWayDraw(image, point, x, y);

    while(x<y)
    {
        if(p<0)
        {
            x++;
            p += (2*x + 1);
        }
        else
        {
            x++;
            y--;
            p+= (2*x + 1 - 2*y);
        }
        eightWayDraw(image, point, x, y);        
    }
}

void floodFill(Image &image, Point &point)
{
    stack<Point> s;
    s.push(point);

    while(!s.empty())
    {
        Point p = s.top(); s.pop();
        if(p.y >=0 && p.y < image.height && p.x>=0 && p.x < image.width && image[p.y][p.x]!=0)
        {
            image[p.y][p.x]=0;
            s.push(newPoint(p.x+1, p.y));
            s.push(newPoint(p.x-1, p.y));
            s.push(newPoint(p.x, p.y+1));
            s.push(newPoint(p.x, p.y-1));
        }
    }
}

void drawPoint(Image &image, Point &p, int r=5)
{
    r = (r&1)?r:r+1;
    
    midPointCircle(image, p, r);
    floodFill(image, p);
}

#endif