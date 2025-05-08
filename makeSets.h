#ifndef MAKE_SETS_H
#define MAKE_SETS_H

#include "boundingBox.h"
#include "segment.h"

#include<vector>
using namespace std;

#define UNDEF -1
#define INVALID -2


void makeRowSets(ImageSegment &comp)
{
    vector<int> setOfIndex(comp.inner.size(), UNDEF);
    int setnum = 0;
    // int heightLimit = (comp.outer.height())*3/4;
    // int widthLimit = (comp.outer.width())*3/4;

    // for(int i=0; i<comp.inner.size(); i++)
    // {
    //     if(comp.inner[i].height()>=heightLimit || comp.inner[i].width()>=widthLimit)
    //     {
    //         setOfIndex[i]=INVALID;
    //     }
    // }

    for(int i=0; i<comp.inner.size(); i++)
    {
        if(setOfIndex[i]==UNDEF)
        {
            vector<int> tempSet;
            tempSet.push_back(i);
            for(int j=i+1; j<comp.inner.size(); j++)
            {
                if(setOfIndex[j]==UNDEF && comp.inner[i].isIntersectingY(comp.inner[j]))
                    tempSet.push_back(j);
            }

            if(tempSet.size()>1)
            {
                comp.rowSets.emplace_back();
                for(int k=0; k<tempSet.size();k++)
                {
                    comp.rowSets[setnum].push_back(&comp.inner[tempSet[k]]);
                    setOfIndex[tempSet[k]] = setnum;
                }
                setnum++;                
            }
            else
            {
                setOfIndex[i]=INVALID;
            }            
        }        
    }
    comp.rowOfIndex = setOfIndex;
}

void makeColSets(ImageSegment &comp)
{
    vector<int> setOfIndex(comp.inner.size(), UNDEF);
    int setnum = 0;

    for(int i=0; i<comp.inner.size(); i++)
    {
        if(setOfIndex[i]==UNDEF && comp.rowOfIndex[i]!=INVALID)
        {
            vector<int> tempSet;
            tempSet.push_back(i);
            for(int j=i+1; j<comp.inner.size(); j++)
            {
                if(setOfIndex[j]==UNDEF && 
                comp.rowOfIndex[j]!=INVALID &&
                comp.rowOfIndex[i]!=comp.rowOfIndex[j] && 
                comp.inner[i].isIntersectingX(comp.inner[j]))
                    tempSet.push_back(j);
            }

            if(tempSet.size()>1)
            {
                bool isValid=true;
                for(int p=0; p<tempSet.size()-1; p++)
                {
                    for(int q=p+1; q<tempSet.size(); q++)
                    {
                        if(!comp.inner[tempSet[p]].isIntersectingX(comp.inner[tempSet[q]]))
                        {
                            isValid = false;
                            break;
                        }
                    }
                    if(!isValid)
                        break;
                }

                if(isValid)
                {
                    comp.colSets.emplace_back();
                    for(int k=0; k<tempSet.size();k++)
                    {
                        comp.colSets[setnum].push_back(&comp.inner[tempSet[k]]);
                        setOfIndex[tempSet[k]] = setnum;
                    }
                    setnum++; 
                }
                else
                    setOfIndex[i] = INVALID;                               
            }
            else
            {
                setOfIndex[i]=INVALID;
            }            
        }
    }
    comp.colOfIndex = setOfIndex;
}

void makeSets(ImageSegment &comp)
{
    makeRowSets(comp);
    makeColSets(comp);
}

void trimBoxesInSet(ImageSegment &comp)
{
    for(int i=0;i<comp.rowSets.size(); i++)
    {
        int maxup = comp.rowSets[i][0]->miny, mindown = comp.rowSets[i][0]->maxy;
        for(int j=1;j<comp.rowSets[i].size();j++)
        {
            if(comp.rowSets[i][j]->miny > maxup)
                maxup = comp.rowSets[i][j]->miny;
            if(comp.rowSets[i][j]->maxy < mindown)
                mindown = comp.rowSets[i][j]->maxy;
        }
        for(int j=0;j<comp.rowSets[i].size();j++)
        {
            comp.rowSets[i][j]->miny = maxup;
            comp.rowSets[i][j]->maxy = mindown;
        }
    }

    for(int i=0;i<comp.colSets.size(); i++)
    {
        int maxl = comp.colSets[i][0]->minx, minr = comp.colSets[i][0]->maxx;
        for(int j=1;j<comp.colSets[i].size();j++)
        {
            if(comp.colSets[i][j]->minx > maxl)
                maxl = comp.colSets[i][j]->minx;
            if(comp.colSets[i][j]->maxx < minr)
                minr = comp.colSets[i][j]->maxx;
        }
        for(int j=0;j<comp.colSets[i].size();j++)
        {
            comp.colSets[i][j]->minx = maxl;
            comp.colSets[i][j]->maxx = minr;
        }
        
    }
}

#endif