#include "provided.h"
#include <vector>
using namespace std;

class DeliveryPlannerImpl
{
public:
    DeliveryPlannerImpl(const StreetMap* sm);
    ~DeliveryPlannerImpl();
    DeliveryResult generateDeliveryPlan(
        const GeoCoord& depot,
        const vector<DeliveryRequest>& deliveries,
        vector<DeliveryCommand>& commands,
        double& totalDistanceTravelled) const;
    
private:
    const StreetMap* m_stPtr;
    PointToPointRouter m_ptpr;
    string direction(StreetSegment seg)const
    {
        double angle = angleOfLine(seg);
        if(angle >= 0 && angle < 22.5)
        {
            return "east";
        }
        else if(angle >= 22.5 && angle < 67.5)
        {
            return "northeast";
        }
        else if(angle >= 67.5 && angle < 112.5)
        {
            return "north";
        }
        else if(angle >= 112.5 && angle < 157.5)
        {
            return "northwest";
        }
        else if(angle >= 157.5 && angle < 202.5)
        {
            return "west";
        }
        else if(angle >= 202.5 && angle < 247.5)
        {
            return "southwest";
        }
        else if(angle >= 247.5 && angle < 292.5)
        {
            return "south";
        }
        else if(angle >= 292.5 && angle < 337.5)
        {
            return "southeast";
        }

        else if (angle >= 337.5)
        {
            return "east";
        }
        else
        {
            return "";
        }
    }
    bool isTurn(StreetSegment s1, StreetSegment s2, string& turn) const
    {
        double angle = angleBetween2Lines(s1, s2);
        if (angle < 1 || angle > 359)
        {
            return false;
        }
        else if(angle >= 1 && angle < 180)
        {
            turn = "left";
        }
        else if(angle >= 180 && angle <=359)
        {
            turn = "right";
        }
        return true;
    }
};

DeliveryPlannerImpl::DeliveryPlannerImpl(const StreetMap* sm)
: m_ptpr(sm)
{
    m_stPtr = sm;
}

DeliveryPlannerImpl::~DeliveryPlannerImpl()
{
}

DeliveryResult DeliveryPlannerImpl::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    list<StreetSegment> tempRoute;
    vector<DeliveryRequest> tempVec = deliveries;
    vector<DeliveryCommand> commandVec;
    double distanceTravelled = 0;
    const string dir = "";
    string turn = "";
    bool prevPro = false;
    DeliveryCommand tempCommand;
    DeliveryOptimizer opti(m_stPtr);
    double oldCrow, newCrow;
    opti.optimizeDeliveryOrder(depot, tempVec, oldCrow, newCrow);
    for(int i = 0; i < deliveries.size(); i++)
    {
        if(i == 0)
        {
            m_ptpr.generatePointToPointRoute(depot, deliveries[i].location, tempRoute, distanceTravelled);
        }
        else
        {
            m_ptpr.generatePointToPointRoute(deliveries[i-1].location, deliveries[i].location, tempRoute, distanceTravelled);
        }
        list<StreetSegment>::iterator it;
        it = tempRoute.begin();
        list<StreetSegment>::iterator it2;
        while(it!=tempRoute.end())
        {
            it2 = it;
            it2++;
                if(it2!=tempRoute.end() && it->name == it2->name && it != tempRoute.begin())
                {
            
                    if(it2 != tempRoute.end() && isTurn(*it, *(it2), turn))
                    {
                        prevPro = false;
                        DeliveryCommand tempCommand;
                        tempCommand.initAsTurnCommand(turn, it2->name);
                        commandVec.push_back(tempCommand);
                    }
                }
                
                if(prevPro == false)
                {
                    DeliveryCommand tempCommand;
                    tempCommand.initAsProceedCommand(dir, it->name, distanceEarthMiles(it->start, it->end));
                    commandVec.push_back(DeliveryCommand(tempCommand));
                    prevPro = true;
                }
                if(it->end == deliveries[i].location)
                {
                    DeliveryCommand tempCommand;
                    tempCommand.initAsDeliverCommand(deliveries[i].item);
                    commandVec.push_back(tempCommand);
                }
            it++;
        }
        totalDistanceTravelled+=distanceTravelled;
    }
    
    distanceTravelled = 0;
    m_ptpr.generatePointToPointRoute(deliveries.back().location, depot, tempRoute, distanceTravelled);
    list<StreetSegment>::iterator it;
    it = tempRoute.begin();
    list<StreetSegment>::iterator it2;
    while(it!=tempRoute.end())
    {
        it2 = it;
        it2++;
            if(it2!=tempRoute.end() && it->name == it2->name && it != tempRoute.begin())
            {
        
                if(it2 != tempRoute.end() && isTurn(*it, *(it2), turn))
                {
                    prevPro = false;
                    DeliveryCommand tempCommand;
                    tempCommand.initAsTurnCommand(turn, it2->name);
                    commandVec.push_back(tempCommand);
                }
            }
            
            if(prevPro == false)
            {
                DeliveryCommand tempCommand;
                tempCommand.initAsProceedCommand(dir, it->name, distanceEarthMiles(it->start, it->end));
                commandVec.push_back(DeliveryCommand(tempCommand));
                prevPro = true;
            }
        it++;
    }
    totalDistanceTravelled+=distanceTravelled;
    commands = commandVec;
    return DELIVERY_SUCCESS;
}


//******************** DeliveryPlanner functions ******************************

// These functions simply delegate to DeliveryPlannerImpl's functions.
// You probably don't want to change any of this code.

DeliveryPlanner::DeliveryPlanner(const StreetMap* sm)
{
    m_impl = new DeliveryPlannerImpl(sm);
}

DeliveryPlanner::~DeliveryPlanner()
{
    delete m_impl;
}

DeliveryResult DeliveryPlanner::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    return m_impl->generateDeliveryPlan(depot, deliveries, commands, totalDistanceTravelled);
}

