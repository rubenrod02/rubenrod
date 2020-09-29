#include "provided.h"
#include "ExpandableHashMap.h"
#include <set>
#include <list>
#include <queue>
using namespace std;

class PointToPointRouterImpl
{
public:
    PointToPointRouterImpl(const StreetMap* sm);
    ~PointToPointRouterImpl();
    DeliveryResult generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const;
    
private:
    const StreetMap* m_smPtr;
};

PointToPointRouterImpl::PointToPointRouterImpl(const StreetMap* sm)
{
    m_smPtr = sm;
}

PointToPointRouterImpl::~PointToPointRouterImpl()
{
}

DeliveryResult PointToPointRouterImpl::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    route.clear();
    totalDistanceTravelled = 0;
    if(start == end)
    {
        return DELIVERY_SUCCESS;
    }
    
    vector<StreetSegment> streetVec;
    if(m_smPtr->getSegmentsThatStartWith(start, streetVec) == false || m_smPtr->getSegmentsThatStartWith(end, streetVec) == false)
    {
        return BAD_COORD;
    }
    
    priority_queue <pair<double, GeoCoord>> geos;
    GeoCoord currentGeo;
    vector<StreetSegment> tempVec;
    pair<double, GeoCoord> tempPair;
    set<GeoCoord> visitedGeo;
    bool isFound = false;
    double distToCurr = 0;
    ExpandableHashMap<GeoCoord, GeoCoord> previousPoint;
    geos.push(pair<double,GeoCoord>(distanceEarthMiles(start, end), start));
    visitedGeo.insert(start);
    
    while(geos.empty() == false)
    {
        currentGeo = geos.top().second;
        distToCurr = abs(abs(geos.top().first) - abs(distanceEarthMiles(currentGeo, end)));
        geos.pop();
        streetVec.clear();
        m_smPtr->getSegmentsThatStartWith(currentGeo, streetVec);
        
        for(auto it = streetVec.begin(); it != streetVec.end(); it++)
        {
            if(visitedGeo.end() == visitedGeo.find(it->end))
            {
                tempPair.first=distToCurr+distanceEarthMiles(currentGeo, it->end)+distanceEarthMiles(it->end, end);
                tempPair.first*=-1;
                tempPair.second = it->end;
                geos.push(tempPair);
                visitedGeo.insert(it->end);
                previousPoint.associate(it->end, currentGeo);
            }
        }
        
        
        if(currentGeo == end)
        {
            isFound = true;
            StreetSegment tempSeg;
            tempSeg.start = *previousPoint.find(currentGeo);
            tempSeg.end = currentGeo;
            bool isRoute = false;
            
            while(isRoute == false)
            {
                tempSeg.start = *previousPoint.find(tempSeg.end);
                
                route.push_back(tempSeg);
                if(tempSeg.start == start)
                {
                    isRoute = true;
                }
                 tempSeg.end = *previousPoint.find(tempSeg.end);
            }
            list<StreetSegment>backwardRoute(route);
            auto it = backwardRoute.end();
            it--;
            
            for(auto it2 = route.begin(); it2 != route.end(); it2++)
            {
                totalDistanceTravelled+=distanceEarthMiles(it2->end, it2->start);
                *it2 = *it;
                it--;
            }
            
            for (auto it = route.begin(); it != route.end(); it++)
            {
                m_smPtr->getSegmentsThatStartWith(it->start, tempVec);
                for(auto it2 = tempVec.begin(); it2 != tempVec.end(); it2++)
                {
                    if(it->end.latitudeText != it2->end.latitudeText && it->end.longitudeText != it2->end.longitudeText)
                    {
                        it->name = it2->name;
                        break;
                    }
                }
            }
            return DELIVERY_SUCCESS;
        }
    }
    
    return NO_ROUTE;
}

//******************** PointToPointRouter functions ***************************

// These functions simply delegate to PointToPointRouterImpl's functions.
// You probably don't want to change any of this code.

PointToPointRouter::PointToPointRouter(const StreetMap* sm)
{
    m_impl = new PointToPointRouterImpl(sm);
}

PointToPointRouter::~PointToPointRouter()
{
    delete m_impl;
}

DeliveryResult PointToPointRouter::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    return m_impl->generatePointToPointRoute(start, end, route, totalDistanceTravelled);
}

