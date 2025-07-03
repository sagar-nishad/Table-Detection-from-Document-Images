#ifndef TABLE_DETECTION_H
#define TABLE_DETECTION_H

#include "Image.h"
#include "isauvola.h"
#include "rlsa.h"
#include "edgeDetection.h"
#include "boundingBox.h"
#include "Point.h"
#include "segment.h"
#include "makeSets.h"
#include "pbmio.h"

#include<vector>
#include<stack>
#include<utility>
#include<algorithm>
using namespace std;

#define RLSA_PARAM_VERTICAL 3/2
#define RLSA_PARAM_HORIZONTAL 2
#define HRLSA_PARAM 2

vector<boundingBox> getBoundingBoxes(Image &img, int H);
vector<boundingBox> removeOverlappingBoxes(vector<boundingBox> &v);
vector<ImageSegment> combineBoxes(vector<boundingBox> &outerBBoxes, vector<boundingBox> &innerBBoxes, int avgh);
void calcCenterPoints(ImageSegment &segment);
void removeLines(Image &img, vector<boundingBox> &outerBBoxes, int H);
void filterInnerBBoxes(ImageSegment &segment,int H);
bool isTable(ImageSegment &segment, Image &img);

void saveIntermediateImages(Image &isauvolaImg, Image &rlsaImg, Image &hrlsaImg, vector<boundingBox> &outerBBoxes, vector<boundingBox> &innerBBoxes, vector<ImageSegment> &segments, int H);

vector<ImageSegment> detectTable(Image &img)
{
    int H = isauvola(img); //binarizes the image 
                           //and returns estimated character height

    // cout << "estimated character height: " << H << "\n";
    
    Image rlsaImg = img.getCopy(), hrlsaImg = img.getCopy();

    Image checkDensity = img.getCopy();     //making a copy to check the density of the segments

    rlsa(rlsaImg, H * RLSA_PARAM_HORIZONTAL, H * RLSA_PARAM_VERTICAL);
    vector<boundingBox> outerBBoxes = getBoundingBoxes(rlsaImg, H);
    outerBBoxes = removeOverlappingBoxes(outerBBoxes);
    removeLines(hrlsaImg, outerBBoxes, H);
    hrlsa(hrlsaImg, H * HRLSA_PARAM);
    vector<boundingBox> innerBBoxes = getBoundingBoxes(hrlsaImg, H);

    vector<ImageSegment> segments = combineBoxes(outerBBoxes, innerBBoxes, H);
    
    for(int i=0;i<segments.size(); i++)
    {
        filterInnerBBoxes(segments[i], H);
        makeSets(segments[i]);
        trimBoxesInSet(segments[i]);

        // cout<<"\nSegment "<<i+1<<" ";
        // segments[i].outer.printBoundingBoxCoords();

        // cout << "segment " << i+1 << " : " << " rows: " << segments[i].rowSets.size() << " cols: " << segments[i].colSets.size() << "\n";

        calcCenterPoints(segments[i]);

        
        if(isTable(segments[i], img))
            segments[i].isTable = true;
        else
            segments[i].isTable = false; 
        
    }

    #ifdef SAVE_INTERMEDIATE
    saveIntermediateImages(img, rlsaImg, hrlsaImg, outerBBoxes, innerBBoxes, segments, H);
    #endif

    return segments;
}

void filterInnerBBoxes(ImageSegment &segment,int H)
{
    int heightULimit = H*5/2;
    int widthULimit = (segment.outer.width())*3/4;
    int heightLLimit = H/2;
    int widthLLimit = H/2;
    for (auto i = segment.inner.begin(); i != segment.inner.end(); i++) 
    {
        if ((*i).height() >= heightULimit || (*i).width() >= widthULimit ||
            (*i).height() <= heightLLimit || (*i).width() <= widthLLimit )
        { 
            segment.inner.erase(i); 
            i--; 
        }
    }
}
bool isTable(ImageSegment &segment, Image &img)
{   
    Image checkDensity = img.getCopy();
    int totSize, densityCounter = 0;
    if(segment.rowSets.size()<2 || segment.colSets.size()<2)
        return false;

    
    int prev_y=-1;
    vector<vector<int>> rows;
    for(int i=0;i<segment.points.size();i++)
    {
        if(segment.points[i].y != prev_y)
        {
            rows.emplace_back();
            prev_y = segment.points[i].y;
        }
        rows.back().push_back(segment.points[i].x);
    }   

    //cout << "rows = " << segment.rowSets.size() << " " << rows.size() << "\n";

    long sum=0;
    for(int i=0;i<rows.size()-1;i++)
    {
        for(int j=i+1;j<rows.size(); j++)
        {
            for(int p=0; p<rows[i].size();p++)
            {
                for(int q=0;q<rows[j].size();q++)
                {
                    if(rows[i][p]==rows[j][q])
                    {
                        sum++;
                    }
                }
            }
        }
    }

    long maxPossibleSum = ((rows.size()*(rows.size()-1))/2)*segment.colSets.size();
    //cout << "sum = " << sum << "; maxsum = " << maxPossibleSum << "\n";
    float score = (float)(sum)/maxPossibleSum;
    //float score = (float)(segment.points.size())/(segment.rowSets.size()*segment.colSets.size());

    // cout << "score = " << score;

    for(int ver = segment.outer.miny; ver<=segment.outer.maxy;ver++){
        for(int hor=segment.outer.minx; hor<=segment.outer.maxx;hor++){
            //cout<<static_cast<int>(checkDensity.a[p][q])<<" ";
            if(checkDensity[ver][hor] == (unsigned char)0){    
                densityCounter++;
            }
        }
    }
    
    totSize =  (segment.outer.maxy-segment.outer.miny)* (segment.outer.maxx-segment.outer.minx);
    float densityFactor =  float(densityCounter)/totSize;
    // cout <<"  Total Size= "<<totSize <<" #Active pixels= "<<densityCounter<< "   density = " <<densityFactor<<" ---- ";

    if(score < 0.5 || (score>0.5 and densityFactor>0.1)){
        // cout<<"Not a Table"<<endl;
        return false;
    }
    // cout<<"This is a Table"<<endl;
    return true;
}

void removeLines(Image &img, vector<boundingBox> &outerBBoxes, int H)
{   
    Image imgCpy = img.getCopy();
    for(int i=0; i<outerBBoxes.size(); i++)
    {
        int hlimit = 3*H, vlimit = 3*H;
        int previousWhite, k;
        
        for(int p = outerBBoxes[i].miny; p<=outerBBoxes[i].maxy;p++)
        {
            previousWhite = outerBBoxes[i].minx-1;
            for(int q=outerBBoxes[i].minx; q<=outerBBoxes[i].maxx;q++)
            {
                if(imgCpy[p][q] == 255)
                {   
                    if((q-previousWhite) > hlimit)
                    {
                        k = previousWhite+1;
                        while(k<q)
                        {
                            img[p][k] = 255;
                            k++;
                        }                        
                    }
                    previousWhite = q;
                }
            }
            if(imgCpy[p][outerBBoxes[i].maxx]==0)
            {
                if((outerBBoxes[i].maxx-previousWhite) > hlimit)
                {
                    k = previousWhite+1;
                    while(k<outerBBoxes[i].maxx)
                    {
                        img[p][k] = 255;
                        k++;
                    } 
                }
            }
        }

        
        for(int q=outerBBoxes[i].minx; q<=outerBBoxes[i].maxx;q++)
        {
            previousWhite = outerBBoxes[i].miny-1;
            for(int p = outerBBoxes[i].miny; p<=outerBBoxes[i].maxy;p++)
            {
                if(imgCpy[p][q] == 255)
                {
                    if((p-previousWhite) > vlimit)
                    {
                        k = previousWhite+1;
                        while(k<p)
                        {
                            img[k][q] = 255;
                            k++;
                        }                        
                    }
                    previousWhite = p;
                }
            }
            if(imgCpy[outerBBoxes[i].maxy][q]==0)
            {
                if((outerBBoxes[i].maxy-previousWhite) > vlimit)
                {
                    k = previousWhite+1;
                    while(k<outerBBoxes[i].maxy)
                    {
                        img[k][q] = 255;
                        k++;
                    } 
                }
            }
        }
    }
}
/*void removeLines(Image &img, vector<boundingBox> &outerBBoxes)
{
     
    for(int i=0; i<outerBBoxes.size(); i++)
    {
        int hlimit = (outerBBoxes[i].width()*4)/5, vlimit = (outerBBoxes[i].height()*4)/5;
        for(int y=outerBBoxes[i].miny; y<=outerBBoxes[i].maxy; y++)
        {
            int blackCount = 0;
            for(int x=outerBBoxes[i].minx; x<=outerBBoxes[i].maxx; x++)
            {
                if(img[y][x]==0) blackCount++;
            }
            if(blackCount > hlimit)
            {
                for(int x=outerBBoxes[i].minx; x<=outerBBoxes[i].maxx; x++)
                    img[y][x] = 255;
            }
        }
        for(int x=outerBBoxes[i].minx; x<=outerBBoxes[i].maxx; x++)
        {
            int blackCount = 0;
            for(int y=outerBBoxes[i].miny; y<=outerBBoxes[i].maxy; y++)
            {
                if(img[y][x]==0) blackCount++;
            }
            if(blackCount > vlimit)
            {
                for(int y=outerBBoxes[i].miny; y<=outerBBoxes[i].maxy; y++)
                    img[y][x] = 255;
            }
        }
    }
}*/

void calcCenterPoints(ImageSegment &segment)
{
    for(int i=0;i<segment.colSets.size(); i++)
    {
        for(int j=0;j<segment.colSets[i].size();j++)
        {
            segment.points.emplace_back();
            segment.points.back().x = (segment.colSets[i][j]->minx + segment.colSets[i][j]->maxx)/2;
            segment.points.back().y = (segment.colSets[i][j]->miny + segment.colSets[i][j]->maxy)/2;    
        }
    }

    // sort the points row and column wise
    sort(segment.points.begin(), segment.points.end());
    // remove duplicate points (if any)
    segment.points.erase(unique(segment.points.begin(), segment.points.end()), segment.points.end());
}

vector<boundingBox> getBoundingBoxes(Image &img, int H)
{
    Image image = img.getCopy();
    edge(image);

	int height = image.height, width = image.width;

	vector<vector<int>> visited(height, vector<int>(width,0));
	vector<boundingBox> v;
	
	for(int i=0; i<height; i++)
    {
        for(int j=0; j<width; j++)
        {
            if(image[i][j]==0 && visited[i][j]==0)
            {                
				stack<pair<int, int>> s;
				s.push(make_pair(i,j));

				boundingBox b; b.minx = width; b.maxx = -1; b.miny = height; b.maxy = -1;

				while(!s.empty())
				{					
					pair<int, int> index = s.top(); s.pop();
					int y=index.first, x = index.second;
					//cout << x << ", " << y << "\n";

					if(image[y][x]==0 && visited[y][x]==0)
					{
						visited[y][x]=1;
						for(int p=y-1;p<=y+1;p++)
						{
							for(int q=x-1;q<=x+1;q++)
							{
								if(!(p==y && q==x) && p>=0 && p<height && q>=0 && q<width && visited[p][q]==0 && image[p][q]==0)
									s.push(make_pair(p,q));
							}
						}

						if(y<b.miny) b.miny=y;
						else if(y>b.maxy) b.maxy=y;
						if(x<b.minx) b.minx=x;
						else if(x>b.maxx) b.maxx=x;
					}
				}
                if(!(b.width() < H/2 || b.height() < H/2)) // remove small boxes
				    v.push_back(b);				
            }
        }
    }
	return v;
}

bool compareBoxSize(boundingBox &a, boundingBox &b)
{
    return a.width()*a.height() > b.width()*b.height();
}

vector<boundingBox> removeOverlappingBoxes(vector<boundingBox> &v)
{
	sort(v.begin(), v.end(), compareBoxSize);
	vector<boundingBox> result;
	result.push_back(v[0]);
	for(int i=1; i<v.size();i++)
	{
		bool inside=false;
		for(int j=0;j<result.size();j++)
		{
			if(v[i].isInside(result[j]))
			{
				inside = true;
				break;
			}
		}

		if(!inside)
			result.push_back(v[i]);
	}

	return result;
}

vector<ImageSegment> combineBoxes(vector<boundingBox> &outerBBoxes, vector<boundingBox> &innerBBoxes, int avgh)
{
    vector<ImageSegment> segments;
    for(int i=0; i<outerBBoxes.size();i++) // remove smaller outerBBoxes
    {
        if(!(outerBBoxes[i].maxx-outerBBoxes[i].minx < 5*avgh || outerBBoxes[i].maxy-outerBBoxes[i].miny < 3*avgh))
        {
            segments.emplace_back();
            segments.back().outer = outerBBoxes[i];
        }
    }

    // attach the innerBBoxes
    for(int i=0; i<innerBBoxes.size(); i++)
    {
        if(!(innerBBoxes[i].width() < avgh/2 || innerBBoxes[i].height() < avgh/2)) // remove smaller innerBBoxes
        {
            // search for the component the subbox belongs to
            for(int j=0; j<segments.size(); j++)
            {
                // if subbox is comletely inside a big box, put it in  its component
                if(innerBBoxes[i].isInside(segments[j].outer))
                {
                    segments[j].inner.push_back(innerBBoxes[i]);
                    break;
                }
            }
        }
    }

    return segments;
}

void saveIntermediateImages(Image& isauvolaImg, Image &rlsaImg, Image &hrlsaImg, vector<boundingBox>& outerBBoxes, vector<boundingBox>& innerBBoxes, vector<ImageSegment>& segments, int H)
{
    // write_pbm(image_filename+"-isauvola.pbm", isauvolaImg);
    // write_pbm(image_filename+"-rlsa.pbm", rlsaImg);
    // write_pbm(image_filename+"-hrlsa.pbm", hrlsaImg);

    Image outerbbimg(isauvolaImg.width, isauvolaImg.height, 255);
    boundingBox::drawBoundingBoxes(outerbbimg, outerBBoxes);
    // write_pbm(image_filename+"-outer-boxes.pbm", outerbbimg);

    // for(int i=0; i<segments.size();i++){
    //     cout<<"Segment "<<i+1<<" ";
    //     segments[i].outer.printBoundingBoxCoords();
    // }

    Image innerbbimg(isauvolaImg.width, isauvolaImg.height, 255);
    boundingBox::drawBoundingBoxes(innerbbimg, innerBBoxes);
    // write_pbm(image_filename+"-inner-boxes.pbm", innerbbimg);

    Image rowColGroupImg(isauvolaImg.width, isauvolaImg.height, 255);
    Image pointsImg(isauvolaImg.width, isauvolaImg.height, 255);
    for(int i=0; i<segments.size();i++)
    {
        boundingBox::drawBoundingBox(rowColGroupImg, segments[i].outer);
        for(int j=0; j<segments[i].colSets.size(); j++)
        {
            for(int k=0;k<segments[i].colSets[j].size(); k++)
                boundingBox::drawBoundingBox(rowColGroupImg, *segments[i].colSets[j][k]);    
        }

        for(int j=0;j<segments[i].points.size();j++)
            drawPoint(pointsImg, segments[i].points[j], H/3);
    }
    // write_pbm(image_filename+"-row-col-groups.pbm", rowColGroupImg);    
    // write_pbm(image_filename+"-lattice.pbm", pointsImg);

}

#endif