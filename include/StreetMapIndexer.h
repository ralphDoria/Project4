#ifndef STREETMAPINDEXER_H
#define STREETMAPINDEXER_H

#include "StreetMap.h"
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <limits>
#include <unordered_set>

class CStreetMapIndexer{
    private:
        struct SImplementation;
        std::unique_ptr<SImplementation> DImplementation;

    public:
        CStreetMapIndexer(std::shared_ptr<CStreetMap> streetmap);
        ~CStreetMapIndexer();
        std::size_t NodeCount() const noexcept;
        std::size_t WayCount() const noexcept;
        std::shared_ptr<CStreetMap::SNode> SortedNodeByIndex(std::size_t index) const noexcept;
        std::shared_ptr<CStreetMap::SWay> SortedWayByIndex(std::size_t index) const noexcept;
        std::unordered_set<std::shared_ptr<CStreetMap::SWay>> WaysInRange(const CStreetMap::SLocation &lowerleft, const CStreetMap::SLocation &upperright) const noexcept;
        std::unordered_set<std::shared_ptr<CStreetMap::SWay>> WaysByNodeID(CStreetMap::TNodeID node) const noexcept;
};

#endif
