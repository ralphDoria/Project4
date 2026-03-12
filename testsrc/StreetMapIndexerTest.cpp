#include <gtest/gtest.h>
#include "MockStreetMap.h"
#include "StreetMapIndexer.h"

#include <iostream>
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
        if(Node && LastNode){
            EXPECT_LT(LastNode->ID(), Node->ID());
        }
        LastNode = Node;
    }
    EXPECT_EQ(Indexer.SortedNodeByIndex(Indexer.NodeCount()),nullptr);
    std::shared_ptr<CStreetMap::SWay> LastWay;
    for(size_t Index = 0; Index < Indexer.WayCount(); Index++){
        auto Way = Indexer.SortedWayByIndex(Index);
        if(Way && LastWay){
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
        WayIDs.insert(Way->ID());
    }
    // 1st, 2nd, A St., B St.
    for(CStreetMap::TWayID WayID : {105, 205, 2407, 3408}){
        EXPECT_TRUE(WayIDs.contains(WayID));
    }

}

