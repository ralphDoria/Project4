#include "StreetMapIndexer.h"

#include <algorithm>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct CStreetMapIndexer::SImplementation{
    std::shared_ptr<CStreetMap> DStreetMap;

    std::vector<std::shared_ptr<CStreetMap::SNode>> DNodes;
    std::vector<std::shared_ptr<CStreetMap::SWay>> DWays;

    std::vector<std::shared_ptr<CStreetMap::SNode>> DSortedNodes;
    std::vector<std::shared_ptr<CStreetMap::SWay>> DSortedWays;

    std::unordered_map<CStreetMap::TNodeID, std::unordered_set<std::shared_ptr<CStreetMap::SWay>>> DWaysByNodeID;

    explicit SImplementation(std::shared_ptr<CStreetMap> streetmap)
        : DStreetMap(std::move(streetmap)){

        if(!DStreetMap){
            return;
        }

        DNodes.reserve(DStreetMap->NodeCount());
        for(std::size_t Index = 0; Index < DStreetMap->NodeCount(); Index++){
            auto Node = DStreetMap->NodeByIndex(Index);
            if(Node){
                DNodes.push_back(Node);
            }
        }

        DWays.reserve(DStreetMap->WayCount());
        for(std::size_t Index = 0; Index < DStreetMap->WayCount(); Index++){
            auto Way = DStreetMap->WayByIndex(Index);
            if(Way){
                DWays.push_back(Way);

                for(std::size_t NodeIndex = 0; NodeIndex < Way->NodeCount(); NodeIndex++){
                    DWaysByNodeID[Way->GetNodeID(NodeIndex)].insert(Way);
                }
            }
        }

        DSortedNodes = DNodes;
        std::sort(DSortedNodes.begin(), DSortedNodes.end(),
                  [](const auto &A, const auto &B){
                      return A->ID() < B->ID();
                  });

        DSortedWays = DWays;
        std::sort(DSortedWays.begin(), DSortedWays.end(),
                  [](const auto &A, const auto &B){
                      return A->ID() < B->ID();
                  });
    }
};

CStreetMapIndexer::CStreetMapIndexer(std::shared_ptr<CStreetMap> streetmap)
    : DImplementation(std::make_unique<SImplementation>(std::move(streetmap))){
}

CStreetMapIndexer::~CStreetMapIndexer() = default;

std::size_t CStreetMapIndexer::NodeCount() const noexcept{
    return DImplementation->DNodes.size();
}

std::size_t CStreetMapIndexer::WayCount() const noexcept{
    return DImplementation->DWays.size();
}

std::shared_ptr<CStreetMap::SNode> CStreetMapIndexer::SortedNodeByIndex(std::size_t index) const noexcept{
    if(index >= DImplementation->DSortedNodes.size()){
        return nullptr;
    }
    return DImplementation->DSortedNodes[index];
}

std::shared_ptr<CStreetMap::SWay> CStreetMapIndexer::SortedWayByIndex(std::size_t index) const noexcept{
    if(index >= DImplementation->DSortedWays.size()){
        return nullptr;
    }
    return DImplementation->DSortedWays[index];
}

std::unordered_set<std::shared_ptr<CStreetMap::SWay>> CStreetMapIndexer::WaysByNodeID(CStreetMap::TNodeID node) const noexcept{
    auto It = DImplementation->DWaysByNodeID.find(node);
    if(It == DImplementation->DWaysByNodeID.end()){
        return {};
    }
    return It->second;
}

std::unordered_set<std::shared_ptr<CStreetMap::SWay>> CStreetMapIndexer::WaysInRange(const CStreetMap::SLocation &lowerleft,
                                                                                      const CStreetMap::SLocation &upperright) const noexcept{
    std::unordered_set<std::shared_ptr<CStreetMap::SWay>> Result;

    for(const auto &Node : DImplementation->DNodes){
        if(!Node){
            continue;
        }

        auto Location = Node->Location();
        if(Location.DLatitude >= lowerleft.DLatitude &&
           Location.DLatitude <= upperright.DLatitude &&
           Location.DLongitude >= lowerleft.DLongitude &&
           Location.DLongitude <= upperright.DLongitude){

            auto Ways = WaysByNodeID(Node->ID());
            Result.insert(Ways.begin(), Ways.end());
        }
    }

    return Result;
}