#include "provided.h"
#include "ExpandableHashMap.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <functional>
using namespace std;

unsigned int hasher(const GeoCoord& g)
{
    return std::hash<string>()(g.latitudeText + g.longitudeText);
}

unsigned int hasher(const string& g)
{
    return std::hash<string>()(g);
}

class StreetMapImpl
{
public:
    StreetMapImpl();
    ~StreetMapImpl();
    bool load(string mapFile);
    bool getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const;
    
private:
    ExpandableHashMap<GeoCoord, vector<StreetSegment>> m_segments;
};

StreetMapImpl::StreetMapImpl()
{
}

StreetMapImpl::~StreetMapImpl()
{
}

bool StreetMapImpl::load(string mapFile)
{
    ifstream inf(mapFile);
    if(!inf)
    {
        cerr << "Error: Cannot open data.txt file." <<endl;
        return false;
    }
    string line;
    string streetName, startLat, startLongi, endLat, endLongi;
    int numTimes;
    while (getline(inf, streetName))
    {
        inf >> numTimes;
        inf.ignore(100000, '\n');
        for (int i = 0; i < numTimes; i++)
        {
            inf >> startLat >> startLongi >> endLat >> endLongi;
            inf.ignore(10000, '\n');
            GeoCoord B(startLat, startLongi);
            GeoCoord E(endLat, endLongi);
            StreetSegment tempSeg;
            vector<StreetSegment>* tempVec = m_segments.find(B);
            if(tempVec != nullptr)
            {
                tempVec->push_back(StreetSegment(B, E, streetName));
            }
            else
            {
                vector<StreetSegment> tempVec;
                tempVec.push_back(StreetSegment(B, E, streetName));
                m_segments.associate(B, tempVec);
            }
            tempVec = m_segments.find(E);
            if(tempVec != nullptr)
            {
                tempVec->push_back(StreetSegment(E, B, streetName));
            }
            else
            {
                vector<StreetSegment> tempVec;
                tempVec.push_back(StreetSegment(E, B, streetName));
                m_segments.associate(E, tempVec);
            }
        }
    }
    return true;
}

bool StreetMapImpl::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
    const vector<StreetSegment>* tempVec = m_segments.find(gc);
    if(tempVec == nullptr)
    {
        return false;
    }
    segs = *tempVec;
    return true;
}

//******************** StreetMap functions ************************************

// These functions simply delegate to StreetMapImpl's functions.
// You probably don't want to change any of this code.

StreetMap::StreetMap()
{
    m_impl = new StreetMapImpl;
}

StreetMap::~StreetMap()
{
    delete m_impl;
}

bool StreetMap::load(string mapFile)
{
    return m_impl->load(mapFile);
}

bool StreetMap::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
   return m_impl->getSegmentsThatStartWith(gc, segs);
}

