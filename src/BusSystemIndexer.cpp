#include "BusSystemIndexer.h"
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <memory>

class CBusSystemIndexer::SImplementation {
public:
    std::shared_ptr<CBusSystem> DSystem;

    std::vector<std::shared_ptr<CBusSystem::SStop>> DStops;
    std::vector<std::shared_ptr<CBusSystem::SRoute>> DRoutes;

    //sorted views
    std::vector<std::shared_ptr<CBusSystem::SStop>> DSortedStops;
    std::vector<std::shared_ptr<CBusSystemIndexer::SRouteIndexer>> DSortedRoutes;

    //fast lookup by route name
    std::unordered_map<std::string, std::shared_ptr<CBusSystemIndexer::SRouteIndexer>> DRouteByName;

    //for each stop id, tells route names
    std::unordered_map<CBusSystem::TStopID, std::unordered_set<std::string>> DStopToRoutes;


    struct CRouteIndexer final : public SRouteIndexer{
        std::shared_ptr<CBusSystem::SRoute> DRoute;

        explicit CRouteIndexer(std::shared_ptr<CBusSystem::SRoute> route)
            : DRoute(std::move(route)) {}

        std::string Name() const noexcept override{
            return DRoute->Name();
        }

        std::size_t StopCount() const noexcept override{
            return DRoute->StopCount();
        }

        TStopID GetStopID(std::size_t index) const noexcept override{
            return DRoute->GetStopID(index);
        }

        std::size_t TripCount() const noexcept override{
            return DRoute->TripCount();
        }

        TStopTime GetStopTime(std::size_t stopindex, std::size_t tripindex) const noexcept override{
            return DRoute->GetStopTime(stopindex, tripindex);
        }

            std::size_t FindStopIndex(TStopID stopid, std::size_t start = 0) const override {
            const std::size_t count = StopCount();
            if (start >= count) {
                return count;
            }
            for (std::size_t i = start; i < count; i++) {
                if (GetStopID(i) == stopid) {
                    return i;
                }
            }
            return count; // "not found" convention: return StopCount()
            }

            std::vector<TStopID> StopIDsSourceDestination(TStopID src, TStopID dest) const override {
            std::vector<TStopID> result;

            const std::size_t count = StopCount();
            if (count == 0) {
                return result;
            }

            const std::size_t srcIndex = FindStopIndex(src, 0);
            if (srcIndex >= count) {
                return result; // src not on route
            }

            const std::size_t destIndex = FindStopIndex(dest, 0);
            if (destIndex >= count) {
                return result; // dest not on route
            }

            // If src comes before dest: forward slice [srcIndex .. destIndex]
            if (srcIndex <= destIndex) {
                result.reserve(destIndex - srcIndex + 1);
                for (std::size_t i = srcIndex; i <= destIndex; i++) {
                    result.push_back(GetStopID(i));
                }
                return result;
            }

            // If src after dest: reverse slice [srcIndex .. destIndex] going backwards
            result.reserve(srcIndex - destIndex + 1);
            for (std::size_t i = srcIndex; i >= destIndex; i--) {
                result.push_back(GetStopID(i));
                if (i == destIndex) { // prevent size_t underflow
                    break;
                }
            }
            return result;
            }
        };

    explicit SImplementation(std::shared_ptr<CBusSystem> system)
        : DSystem(std::move(system)) {

        //pulls the stops and routes from ssytem
        if (DSystem) {
            DStops.reserve(DSystem->StopCount());
            for (std::size_t i = 0; i < DSystem->StopCount(); i++) {
                auto stop = DSystem->StopByIndex(i);
                if (stop) {
                    DStops.push_back(stop);
                }
            }

            DRoutes.reserve(DSystem->RouteCount());
            for (std::size_t i = 0; i < DSystem->RouteCount(); i++) {
                auto route = DSystem->RouteByIndex(i);
                if (route) {
                    DRoutes.push_back(route);
                }
            }
        }

        //build sorted lists
        DSortedStops = DStops;
        std::sort(DSortedStops.begin(), DSortedStops.end(),
                [](const auto &a, const auto &b) {
                    return a->ID() < b->ID();
                });


                DSortedRoutes.clear();
            DSortedRoutes.reserve(DRoutes.size());

            for(const auto &RoutePtr : DRoutes){
                auto RouteIndexerPtr = std::make_shared<CRouteIndexer>(RoutePtr);
                DSortedRoutes.push_back(RouteIndexerPtr);
                DRouteByName[RoutePtr->Name()] = RouteIndexerPtr;
            }


        std::sort(DSortedRoutes.begin(), DSortedRoutes.end(),
                  [](const auto &a, const auto &b) {
                      //sort by route name
                      return a->Name() < b->Name();
                  });

        //build route-by-name and stop->routes indexes
        for (const auto &route : DRoutes) {
            const std::size_t count = route->StopCount();
            for (std::size_t i = 0; i < count; i++) {
                const auto sid = route->GetStopID(i);
                DStopToRoutes[sid].insert(route->Name());
            }
        }
    }
};

CBusSystemIndexer::CBusSystemIndexer(std::shared_ptr<CBusSystem> bussystem)
    : DImplementation(std::make_unique<SImplementation>(std::move(bussystem))) {
}

CBusSystemIndexer::~CBusSystemIndexer() = default;

std::size_t CBusSystemIndexer::StopCount() const  noexcept {
    return DImplementation->DStops.size();
}

std::size_t CBusSystemIndexer::RouteCount() const  noexcept {
    return DImplementation->DRoutes.size();
}

std::shared_ptr<CBusSystem::SStop> CBusSystemIndexer::SortedStopByIndex(std::size_t index) const  noexcept {
    if (index >= DImplementation->DSortedStops.size()) {
        return nullptr;
    }
    return DImplementation->DSortedStops[index];
}

std::shared_ptr<CBusSystemIndexer::SRouteIndexer> CBusSystemIndexer::SortedRouteByIndex(std::size_t index) const noexcept {
    if (index >= DImplementation->DSortedRoutes.size()) {
        return nullptr;
    }
    return DImplementation->DSortedRoutes[index];
}

std::shared_ptr<CBusSystemIndexer::SRouteIndexer> CBusSystemIndexer::RouteByName(const std::string &name) const noexcept {
    auto it = DImplementation->DRouteByName.find(name);
    if (it == DImplementation->DRouteByName.end()) {
        return nullptr;
    }
    return it->second;
}

bool CBusSystemIndexer::RoutesByStopID(CBusSystem::TStopID stopid,
                                      std::unordered_set<std::string> &routes) const noexcept  {
    routes.clear();

    auto it = DImplementation->DStopToRoutes.find(stopid);
    if (it == DImplementation->DStopToRoutes.end()) {
        return false;
    }

    routes = it->second;
    return !routes.empty();
}

bool CBusSystemIndexer::RoutesByStopIDs(CBusSystem::TStopID stopid1,
                                       CBusSystem::TStopID stopid2,
                                       std::unordered_set<std::string> &routes) const noexcept  {
    routes.clear();

    auto it1 = DImplementation->DStopToRoutes.find(stopid1);
    auto it2 = DImplementation->DStopToRoutes.find(stopid2);
    if (it1 == DImplementation->DStopToRoutes.end() || it2 == DImplementation->DStopToRoutes.end()) {
        return false;
    }

    // intersection of routename sets
    const auto &set1 = it1->second;
    const auto &set2 = it2->second;

    //iterate smaller set
    if (set1.size() <= set2.size()) {
        for (const auto &name : set1) {
            if (set2.find(name) != set2.end()) {
                routes.insert(name);
            }
        }
    } else {
        for (const auto &name : set2) {
            if (set1.find(name) != set1.end()) {
                routes.insert(name);
            }
        }
    }

    return !routes.empty();
}

bool CBusSystemIndexer::StopIDsByRoutes(const std::string &route1,
                                       const std::string &route2,
                                       std::unordered_set<CBusSystem::TStopID> &stopids) const noexcept  {
    stopids.clear();

    auto r1 = RouteByName(route1);
    auto r2 = RouteByName(route2);
    if (!r1 || !r2) {
        return false;
    }

    std::unordered_set<CBusSystem::TStopID> s1;
    s1.reserve(r1->StopCount());

    for (std::size_t i = 0; i < r1->StopCount(); i++) {
        s1.insert(r1->GetStopID(i));
    }

    for (std::size_t i = 0; i < r2->StopCount(); i++) {
        auto sid = r2->GetStopID(i);
        if (s1.find(sid) != s1.end()) {
            stopids.insert(sid);
        }
    }

    return !stopids.empty();
}