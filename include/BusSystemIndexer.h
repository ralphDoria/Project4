#ifndef BUSSYSTEMINDEXER_H
#define BUSSYSTEMINDEXER_H
#include "BusSystem.h"
#include <unordered_set>
#include <vector>

class CBusSystemIndexer{
    private:
        struct SImplementation;
        std::unique_ptr<SImplementation> DImplementation;
        
    public:
        using TStopID = CBusSystem::TStopID;
        using SStop = CBusSystem::SStop;
        using SRoute = CBusSystem::SRoute;
        struct SRouteIndexer : public SRoute{
            virtual ~SRouteIndexer(){};
            virtual size_t FindStopIndex(TStopID stopid, size_t start = 0) const = 0;
            virtual std::vector< TStopID > StopIDsSourceDestination(TStopID src, TStopID dest) const = 0;
        };

        CBusSystemIndexer(std::shared_ptr<CBusSystem> bussystem);
        ~CBusSystemIndexer();

        std::size_t StopCount() const noexcept;
        std::size_t RouteCount() const noexcept;
        std::shared_ptr<SStop> SortedStopByIndex(std::size_t index) const noexcept;
        std::shared_ptr<SRouteIndexer> SortedRouteByIndex(std::size_t index) const noexcept;
        std::shared_ptr<SRouteIndexer> RouteByName(const std::string &name) const noexcept;
        bool RoutesByStopID(TStopID stopid, std::unordered_set< std::string > &routes) const noexcept;
        bool RoutesByStopIDs(TStopID src, TStopID dest, std::unordered_set< std::string > &routes) const noexcept;
        bool StopIDsByRoutes(const std::string &route1, const std::string &route2, std::unordered_set< TStopID > &stops) const noexcept;
};

#endif
