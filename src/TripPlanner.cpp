#include "TripPlanner.h"

#include <memory>
#include <unordered_set>
#include <vector>

struct CTripPlanner::SImplementation{
    std::shared_ptr<CBusSystem> DBusSystem;
    std::shared_ptr<CBusSystemIndexer> DBusSystemIndexer;

    SImplementation(std::shared_ptr<CBusSystem> bussystem)
        : DBusSystem(std::move(bussystem)),
          DBusSystemIndexer(std::make_shared<CBusSystemIndexer>(DBusSystem)){
    }
};

CTripPlanner::CTripPlanner(std::shared_ptr<CBusSystem> bussystem)
    : DImplementation(std::make_unique<SImplementation>(std::move(bussystem))){
}

CTripPlanner::~CTripPlanner() = default;

std::shared_ptr<CBusSystemIndexer> CTripPlanner::BusSystemIndexer() const{
    return DImplementation->DBusSystemIndexer;
}

std::shared_ptr<CTripPlanner::SRoute> CTripPlanner::FindDirectRouteLeaveTime(TStopID src, TStopID dest, TStopTime leaveat) const{
    auto Indexer = DImplementation->DBusSystemIndexer;
    std::shared_ptr<SRoute> BestRoute = nullptr;
    TStopTime BestArrival;
    bool Found = false;

    std::unordered_set<std::string> RouteNames;
    if(!Indexer->RoutesByStopIDs(src, dest, RouteNames)){
        return nullptr;
    }

    for(const auto &RouteName : RouteNames){
        auto Route = Indexer->RouteByName(RouteName);
        if(!Route){
            continue;
        }

        auto SrcIndex = Route->FindStopIndex(src);
        auto DestIndex = Route->FindStopIndex(dest, SrcIndex + 1);

        if(SrcIndex >= Route->StopCount() || DestIndex >= Route->StopCount()){
            continue;
        }

        for(std::size_t TripIndex = 0; TripIndex < Route->TripCount(); TripIndex++){
            auto DepartTime = Route->GetStopTime(SrcIndex, TripIndex);
            auto ArrivalTime = Route->GetStopTime(DestIndex, TripIndex);

            if(DepartTime < leaveat){
                continue;
            }

            if(!Found || ArrivalTime < BestArrival){
                BestRoute = Route;
                BestArrival = ArrivalTime;
                Found = true;
            }
        }
    }

    return BestRoute;
}

std::shared_ptr<CTripPlanner::SRoute> CTripPlanner::FindDirectRouteArrivalTime(TStopID src, TStopID dest, TStopTime arriveby) const{
    auto Indexer = DImplementation->DBusSystemIndexer;
    std::shared_ptr<SRoute> BestRoute = nullptr;
    TStopTime BestDeparture;
    bool Found = false;

    std::unordered_set<std::string> RouteNames;
    if(!Indexer->RoutesByStopIDs(src, dest, RouteNames)){
        return nullptr;
    }

    for(const auto &RouteName : RouteNames){
        auto Route = Indexer->RouteByName(RouteName);
        if(!Route){
            continue;
        }

        auto SrcIndex = Route->FindStopIndex(src);
        auto DestIndex = Route->FindStopIndex(dest, SrcIndex + 1);

        if(SrcIndex >= Route->StopCount() || DestIndex >= Route->StopCount()){
            continue;
        }

        for(std::size_t TripIndex = 0; TripIndex < Route->TripCount(); TripIndex++){
            auto DepartTime = Route->GetStopTime(SrcIndex, TripIndex);
            auto ArrivalTime = Route->GetStopTime(DestIndex, TripIndex);

            if(ArrivalTime > arriveby){
                continue;
            }

            if(!Found || BestDeparture < DepartTime){
                BestRoute = Route;
                BestDeparture = DepartTime;
                Found = true;
            }
        }
    }

    return BestRoute;
}

bool CTripPlanner::FindRouteLeaveTime(TStopID src, TStopID dest, TStopTime leaveat, TTravelPlan &plan) const{
    plan.clear();

    auto DirectRoute = FindDirectRouteLeaveTime(src, dest, leaveat);
    if(DirectRoute){
        auto IndexerRoute = DImplementation->DBusSystemIndexer->RouteByName(DirectRoute->Name());
        auto SrcIndex = IndexerRoute->FindStopIndex(src);
        auto DestIndex = IndexerRoute->FindStopIndex(dest, SrcIndex + 1);

        bool FoundTrip = false;
        std::size_t BestTrip = 0;
        TStopTime BestArrival;

        for(std::size_t TripIndex = 0; TripIndex < IndexerRoute->TripCount(); TripIndex++){
            auto DepartTime = IndexerRoute->GetStopTime(SrcIndex, TripIndex);
            auto ArrivalTime = IndexerRoute->GetStopTime(DestIndex, TripIndex);

            if(DepartTime < leaveat){
                continue;
            }

            if(!FoundTrip || ArrivalTime < BestArrival){
                BestTrip = TripIndex;
                BestArrival = ArrivalTime;
                FoundTrip = true;
            }
        }

        if(FoundTrip){
            plan.push_back({IndexerRoute->GetStopTime(SrcIndex, BestTrip), src, IndexerRoute->Name()});
            plan.push_back({IndexerRoute->GetStopTime(DestIndex, BestTrip), dest, ""});
            return true;
        }
    }

    auto Indexer = DImplementation->DBusSystemIndexer;

    for(std::size_t FirstRouteIndex = 0; FirstRouteIndex < Indexer->RouteCount(); FirstRouteIndex++){
        auto FirstRoute = Indexer->SortedRouteByIndex(FirstRouteIndex);
        if(!FirstRoute){
            continue;
        }

        auto SrcIndex = FirstRoute->FindStopIndex(src);
        if(SrcIndex >= FirstRoute->StopCount()){
            continue;
        }

        for(std::size_t TransferIndex = SrcIndex + 1; TransferIndex < FirstRoute->StopCount(); TransferIndex++){
            auto TransferStop = FirstRoute->GetStopID(TransferIndex);

            std::unordered_set<std::string> SecondRouteNames;
            if(!Indexer->RoutesByStopIDs(TransferStop, dest, SecondRouteNames)){
                continue;
            }

            for(std::size_t FirstTrip = 0; FirstTrip < FirstRoute->TripCount(); FirstTrip++){
                auto FirstDepart = FirstRoute->GetStopTime(SrcIndex, FirstTrip);
                auto TransferArrival = FirstRoute->GetStopTime(TransferIndex, FirstTrip);

                if(FirstDepart < leaveat){
                    continue;
                }

                for(const auto &SecondRouteName : SecondRouteNames){
                    auto SecondRoute = Indexer->RouteByName(SecondRouteName);
                    if(!SecondRoute){
                        continue;
                    }

                    auto TransferIndex2 = SecondRoute->FindStopIndex(TransferStop);
                    auto DestIndex2 = SecondRoute->FindStopIndex(dest, TransferIndex2 + 1);

                    if(TransferIndex2 >= SecondRoute->StopCount() || DestIndex2 >= SecondRoute->StopCount()){
                        continue;
                    }

                    for(std::size_t SecondTrip = 0; SecondTrip < SecondRoute->TripCount(); SecondTrip++){
                        auto SecondDepart = SecondRoute->GetStopTime(TransferIndex2, SecondTrip);
                        auto FinalArrival = SecondRoute->GetStopTime(DestIndex2, SecondTrip);

                        if(SecondDepart < TransferArrival){
                            continue;
                        }

                        plan.push_back({FirstDepart, src, FirstRoute->Name()});
                        plan.push_back({SecondDepart, TransferStop, SecondRoute->Name()});
                        plan.push_back({FinalArrival, dest, ""});
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

bool CTripPlanner::FindRouteArrivalTime(TStopID src, TStopID dest, TStopTime arriveby, TTravelPlan &plan) const{
    plan.clear();

    auto DirectRoute = FindDirectRouteArrivalTime(src, dest, arriveby);
    if(DirectRoute){
        auto IndexerRoute = DImplementation->DBusSystemIndexer->RouteByName(DirectRoute->Name());
        auto SrcIndex = IndexerRoute->FindStopIndex(src);
        auto DestIndex = IndexerRoute->FindStopIndex(dest, SrcIndex + 1);

        bool FoundTrip = false;
        std::size_t BestTrip = 0;
        TStopTime BestDeparture;

        for(std::size_t TripIndex = 0; TripIndex < IndexerRoute->TripCount(); TripIndex++){
            auto DepartTime = IndexerRoute->GetStopTime(SrcIndex, TripIndex);
            auto ArrivalTime = IndexerRoute->GetStopTime(DestIndex, TripIndex);

            if(ArrivalTime > arriveby){
                continue;
            }

            if(!FoundTrip || BestDeparture < DepartTime){
                BestTrip = TripIndex;
                BestDeparture = DepartTime;
                FoundTrip = true;
            }
        }

        if(FoundTrip){
            plan.push_back({IndexerRoute->GetStopTime(SrcIndex, BestTrip), src, IndexerRoute->Name()});
            plan.push_back({IndexerRoute->GetStopTime(DestIndex, BestTrip), dest, ""});
            return true;
        }
    }

    auto Indexer = DImplementation->DBusSystemIndexer;

    bool FoundPlan = false;
    TTravelPlan BestPlan;
    TStopTime BestFirstDeparture;

    for(std::size_t FirstRouteIndex = 0; FirstRouteIndex < Indexer->RouteCount(); FirstRouteIndex++){
        auto FirstRoute = Indexer->SortedRouteByIndex(FirstRouteIndex);
        if(!FirstRoute){
            continue;
        }

        auto SrcIndex = FirstRoute->FindStopIndex(src);
        if(SrcIndex >= FirstRoute->StopCount()){
            continue;
        }

        for(std::size_t TransferIndex = SrcIndex + 1; TransferIndex < FirstRoute->StopCount(); TransferIndex++){
            auto TransferStop = FirstRoute->GetStopID(TransferIndex);

            std::unordered_set<std::string> SecondRouteNames;
            if(!Indexer->RoutesByStopIDs(TransferStop, dest, SecondRouteNames)){
                continue;
            }

            for(const auto &SecondRouteName : SecondRouteNames){
                auto SecondRoute = Indexer->RouteByName(SecondRouteName);
                if(!SecondRoute){
                    continue;
                }

                auto TransferIndex2 = SecondRoute->FindStopIndex(TransferStop);
                auto DestIndex2 = SecondRoute->FindStopIndex(dest, TransferIndex2 + 1);

                if(TransferIndex2 >= SecondRoute->StopCount() || DestIndex2 >= SecondRoute->StopCount()){
                    continue;
                }

                for(std::size_t FirstTrip = 0; FirstTrip < FirstRoute->TripCount(); FirstTrip++){
                    auto FirstDepart = FirstRoute->GetStopTime(SrcIndex, FirstTrip);
                    auto TransferArrival = FirstRoute->GetStopTime(TransferIndex, FirstTrip);

                    for(std::size_t SecondTrip = 0; SecondTrip < SecondRoute->TripCount(); SecondTrip++){
                        auto SecondDepart = SecondRoute->GetStopTime(TransferIndex2, SecondTrip);
                        auto FinalArrival = SecondRoute->GetStopTime(DestIndex2, SecondTrip);

                        if(SecondDepart < TransferArrival){
                            continue;
                        }
                        if(FinalArrival > arriveby){
                            continue;
                        }

                        if(!FoundPlan || BestFirstDeparture < FirstDepart){
                            BestPlan.clear();
                            BestPlan.push_back({FirstDepart, src, FirstRoute->Name()});
                            BestPlan.push_back({SecondDepart, TransferStop, SecondRoute->Name()});
                            BestPlan.push_back({FinalArrival, dest, ""});
                            BestFirstDeparture = FirstDepart;
                            FoundPlan = true;
                        }
                    }
                }
            }
        }
    }

    if(FoundPlan){
        plan = BestPlan;
        return true;
    }

    return false;
}