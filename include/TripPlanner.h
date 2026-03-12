#ifndef TRIPPLANNER_H
#define TRIPPLANNER_H

#include "BusSystem.h"
#include "BusSystemIndexer.h"
#include <memory>

class CTripPlanner{
    private:
        struct SImplementation;
        std::unique_ptr<SImplementation> DImplementation;

    public:
        using TStopID = CBusSystem::TStopID;
        using TStopTime = CBusSystem::TStopTime;
        using SRoute = CBusSystem::SRoute;

        struct STravelStep{
            TStopTime DTime;
            TStopID DStopID;
            std::string DRouteName;
        };
        
        using TTravelPlan = std::vector< STravelStep >;

        CTripPlanner(std::shared_ptr<CBusSystem> bussystem);
        ~CTripPlanner();

        std::shared_ptr<CBusSystemIndexer> BusSystemIndexer() const;

        std::shared_ptr< SRoute > FindDirectRouteLeaveTime(TStopID src, TStopID dest, TStopTime leaveat) const;
        std::shared_ptr< SRoute > FindDirectRouteArrivalTime(TStopID src, TStopID dest, TStopTime arriveby) const;

        bool FindRouteLeaveTime(TStopID src, TStopID dest, TStopTime leaveat, TTravelPlan &plan) const;
        bool FindRouteArrivalTime(TStopID src, TStopID dest, TStopTime arriveby, TTravelPlan &plan) const;
};

#endif
