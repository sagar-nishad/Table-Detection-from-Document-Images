#ifndef SEGMENT_H
#define SEGMENT_H

#include "Image.h"
#include "boundingBox.h"
#include "Point.h"

#include<vector>
using namespace std;

struct ImageSegment
{
    boundingBox outer;
    vector<boundingBox> inner;
    vector<int> rowOfIndex, colOfIndex;
    vector<vector<boundingBox*>> rowSets, colSets; 
    vector<Point> points;
    bool isTable;
};

#endif