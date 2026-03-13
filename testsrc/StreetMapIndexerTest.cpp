#include <gtest/gtest.h>
#include "MockStreetMap.h"
#include "StreetMapIndexer.h"

#include <iostream>
#include <unordered_set>
#include <vector>

using std::cout;
using std::endl;

class StreepMapIndexer : public ::testing::Test{
    protected:
        std::shared_ptr<CMockStreetMap> DStreetMap;

        void SetUp() override{
            DStreetMap = CMockStreetMap::CreateTestStreetMap();
        }

        void TearDown() override{

        }
};

TEST_F(StreepMapIndexer, SimpleTest){
    CStreetMapIndexer Indexer(DStreetMap);

    EXPECT_EQ(Indexer.NodeCount(),DStreetMap->NodeCount());
    EXPECT_EQ(Indexer.WayCount(),DStreetMap->WayCount());

    std::shared_ptr<CStreetMap::SNode> LastNode;
    for(size_t Index = 0; Index < Indexer.NodeCount(); Index++){
        auto Node = Indexer.SortedNodeByIndex(Index);
        ASSERT_NE(Node, nullptr);
        if(LastNode){
            EXPECT_LT(LastNode->ID(), Node->ID());
        }
        LastNode = Node;
    }
    EXPECT_EQ(Indexer.SortedNodeByIndex(Indexer.NodeCount()),nullptr);

    std::shared_ptr<CStreetMap::SWay> LastWay;
    for(size_t Index = 0; Index < Indexer.WayCount(); Index++){
        auto Way = Indexer.SortedWayByIndex(Index);
        ASSERT_NE(Way, nullptr);
        if(LastWay){
            EXPECT_LT(LastWay->ID(), Way->ID());
        }
        LastWay = Way;
    }
    EXPECT_EQ(Indexer.SortedWayByIndex(Indexer.WayCount()),nullptr);

    CStreetMap::SLocation LowerLeft{38.5 + (-0.25)/69.0, -121.7 +(-0.25)/54.2};
    CStreetMap::SLocation UpperRight{38.5 + (0.26)/69.0, -121.7 +(0.26)/54.2};
    auto Ways = Indexer.WaysInRange(LowerLeft,UpperRight);

    std::unordered_set<CStreetMap::TWayID> WayIDs;
    for(auto Way : Ways){
        ASSERT_NE(Way, nullptr);
        WayIDs.insert(Way->ID());
    }

    // 1st, 2nd, A St., B St.
    for(CStreetMap::TWayID WayID : {105, 205, 2407, 3408}){
        EXPECT_TRUE(WayIDs.find(WayID) != WayIDs.end());
    }
}

TEST_F(StreepMapIndexer, WaysByNodeIDValidNode){
    CStreetMapIndexer Indexer(DStreetMap);

    bool FoundNodeWithWays = false;

    for(size_t Index = 0; Index < DStreetMap->NodeCount(); Index++){
        auto Node = DStreetMap->NodeByIndex(Index);
        ASSERT_NE(Node, nullptr);

        auto Ways = Indexer.WaysByNodeID(Node->ID());
        if(!Ways.empty()){
            FoundNodeWithWays = true;

            for(const auto &Way : Ways){
                ASSERT_NE(Way, nullptr);

                bool ContainsNode = false;
                for(size_t NodeIndex = 0; NodeIndex < Way->NodeCount(); NodeIndex++){
                    if(Way->GetNodeID(NodeIndex) == Node->ID()){
                        ContainsNode = true;
                        break;
                    }
                }
                EXPECT_TRUE(ContainsNode);
            }
        }
    }

    EXPECT_TRUE(FoundNodeWithWays);
}

TEST_F(StreepMapIndexer, WaysByNodeIDInvalidNode){
    CStreetMapIndexer Indexer(DStreetMap);

    auto Ways = Indexer.WaysByNodeID(static_cast<CStreetMap::TNodeID>(999999999));
    EXPECT_TRUE(Ways.empty());
}

TEST_F(StreepMapIndexer, WaysByNodeIDMatchesManualComputation){
    CStreetMapIndexer Indexer(DStreetMap);

    for(size_t Index = 0; Index < DStreetMap->NodeCount(); Index++){
        auto Node = DStreetMap->NodeByIndex(Index);
        ASSERT_NE(Node, nullptr);

        std::unordered_set<CStreetMap::TWayID> ExpectedWayIDs;
        for(size_t WayIndex = 0; WayIndex < DStreetMap->WayCount(); WayIndex++){
            auto Way = DStreetMap->WayByIndex(WayIndex);
            ASSERT_NE(Way, nullptr);

            for(size_t NodeIndex = 0; NodeIndex < Way->NodeCount(); NodeIndex++){
                if(Way->GetNodeID(NodeIndex) == Node->ID()){
                    ExpectedWayIDs.insert(Way->ID());
                    break;
                }
            }
        }

        auto ActualWays = Indexer.WaysByNodeID(Node->ID());
        std::unordered_set<CStreetMap::TWayID> ActualWayIDs;
        for(const auto &Way : ActualWays){
            ASSERT_NE(Way, nullptr);
            ActualWayIDs.insert(Way->ID());
        }

        EXPECT_EQ(ActualWayIDs, ExpectedWayIDs);
    }
}

TEST_F(StreepMapIndexer, WaysInRangeEmptyRangeOutsideMap){
    CStreetMapIndexer Indexer(DStreetMap);

    CStreetMap::SLocation LowerLeft{0.0, 0.0};
    CStreetMap::SLocation UpperRight{1.0, 1.0};

    auto Ways = Indexer.WaysInRange(LowerLeft, UpperRight);
    EXPECT_TRUE(Ways.empty());
}

TEST_F(StreepMapIndexer, WaysInRangeWholeMapContainsAllWays){
    CStreetMapIndexer Indexer(DStreetMap);

    double MinLat =  1e100;
    double MaxLat = -1e100;
    double MinLon =  1e100;
    double MaxLon = -1e100;

    for(size_t Index = 0; Index < DStreetMap->NodeCount(); Index++){
        auto Node = DStreetMap->NodeByIndex(Index);
        ASSERT_NE(Node, nullptr);

        auto Location = Node->Location();
        MinLat = std::min(MinLat, Location.DLatitude);
        MaxLat = std::max(MaxLat, Location.DLatitude);
        MinLon = std::min(MinLon, Location.DLongitude);
        MaxLon = std::max(MaxLon, Location.DLongitude);
    }

    CStreetMap::SLocation LowerLeft{MinLat, MinLon};
    CStreetMap::SLocation UpperRight{MaxLat, MaxLon};

    auto Ways = Indexer.WaysInRange(LowerLeft, UpperRight);

    std::unordered_set<CStreetMap::TWayID> ActualWayIDs;
    for(const auto &Way : Ways){
        ASSERT_NE(Way, nullptr);
        ActualWayIDs.insert(Way->ID());
    }

    std::unordered_set<CStreetMap::TWayID> ExpectedWayIDs;
    for(size_t Index = 0; Index < DStreetMap->WayCount(); Index++){
        auto Way = DStreetMap->WayByIndex(Index);
        ASSERT_NE(Way, nullptr);
        ExpectedWayIDs.insert(Way->ID());
    }

    EXPECT_EQ(ActualWayIDs, ExpectedWayIDs);
}

TEST_F(StreepMapIndexer, WaysInRangeSingleNodeBoundaryInclusion){
    CStreetMapIndexer Indexer(DStreetMap);

    auto Node = DStreetMap->NodeByIndex(0);
    ASSERT_NE(Node, nullptr);

    auto Location = Node->Location();
    CStreetMap::SLocation LowerLeft{Location.DLatitude, Location.DLongitude};
    CStreetMap::SLocation UpperRight{Location.DLatitude, Location.DLongitude};

    auto Ways = Indexer.WaysInRange(LowerLeft, UpperRight);
    auto ExpectedWays = Indexer.WaysByNodeID(Node->ID());

    std::unordered_set<CStreetMap::TWayID> ActualWayIDs;
    for(const auto &Way : Ways){
        ASSERT_NE(Way, nullptr);
        ActualWayIDs.insert(Way->ID());
    }

    std::unordered_set<CStreetMap::TWayID> ExpectedWayIDs;
    for(const auto &Way : ExpectedWays){
        ASSERT_NE(Way, nullptr);
        ExpectedWayIDs.insert(Way->ID());
    }

    EXPECT_EQ(ActualWayIDs, ExpectedWayIDs);
}

TEST_F(StreepMapIndexer, WaysInRangeMatchesManualComputation){
    CStreetMapIndexer Indexer(DStreetMap);

    CStreetMap::SLocation LowerLeft{38.5 + (-0.25)/69.0, -121.7 +(-0.25)/54.2};
    CStreetMap::SLocation UpperRight{38.5 + (0.26)/69.0, -121.7 +(0.26)/54.2};

    std::unordered_set<CStreetMap::TWayID> ExpectedWayIDs;

    for(size_t Index = 0; Index < DStreetMap->NodeCount(); Index++){
        auto Node = DStreetMap->NodeByIndex(Index);
        ASSERT_NE(Node, nullptr);

        auto Location = Node->Location();
        if(Location.DLatitude >= LowerLeft.DLatitude &&
           Location.DLatitude <= UpperRight.DLatitude &&
           Location.DLongitude >= LowerLeft.DLongitude &&
           Location.DLongitude <= UpperRight.DLongitude){

            for(size_t WayIndex = 0; WayIndex < DStreetMap->WayCount(); WayIndex++){
                auto Way = DStreetMap->WayByIndex(WayIndex);
                ASSERT_NE(Way, nullptr);

                for(size_t NodeIndex = 0; NodeIndex < Way->NodeCount(); NodeIndex++){
                    if(Way->GetNodeID(NodeIndex) == Node->ID()){
                        ExpectedWayIDs.insert(Way->ID());
                        break;
                    }
                }
            }
        }
    }

    auto ActualWays = Indexer.WaysInRange(LowerLeft, UpperRight);
    std::unordered_set<CStreetMap::TWayID> ActualWayIDs;
    for(const auto &Way : ActualWays){
        ASSERT_NE(Way, nullptr);
        ActualWayIDs.insert(Way->ID());
    }

    EXPECT_EQ(ActualWayIDs, ExpectedWayIDs);
}

TEST_F(StreepMapIndexer, SortedNodeByIndexReturnsAllNodesInOrder){
    CStreetMapIndexer Indexer(DStreetMap);

    std::vector<CStreetMap::TNodeID> SortedIDs;
    for(size_t Index = 0; Index < Indexer.NodeCount(); Index++){
        auto Node = Indexer.SortedNodeByIndex(Index);
        ASSERT_NE(Node, nullptr);
        SortedIDs.push_back(Node->ID());
    }

    std::vector<CStreetMap::TNodeID> ExpectedIDs;
    for(size_t Index = 0; Index < DStreetMap->NodeCount(); Index++){
        auto Node = DStreetMap->NodeByIndex(Index);
        ASSERT_NE(Node, nullptr);
        ExpectedIDs.push_back(Node->ID());
    }
    std::sort(ExpectedIDs.begin(), ExpectedIDs.end());

    EXPECT_EQ(SortedIDs, ExpectedIDs);
}

TEST_F(StreepMapIndexer, SortedWayByIndexReturnsAllWaysInOrder){
    CStreetMapIndexer Indexer(DStreetMap);

    std::vector<CStreetMap::TWayID> SortedIDs;
    for(size_t Index = 0; Index < Indexer.WayCount(); Index++){
        auto Way = Indexer.SortedWayByIndex(Index);
        ASSERT_NE(Way, nullptr);
        SortedIDs.push_back(Way->ID());
    }

    std::vector<CStreetMap::TWayID> ExpectedIDs;
    for(size_t Index = 0; Index < DStreetMap->WayCount(); Index++){
        auto Way = DStreetMap->WayByIndex(Index);
        ASSERT_NE(Way, nullptr);
        ExpectedIDs.push_back(Way->ID());
    }
    std::sort(ExpectedIDs.begin(), ExpectedIDs.end());

    EXPECT_EQ(SortedIDs, ExpectedIDs);
}