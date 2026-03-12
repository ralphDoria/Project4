#include "MockStreetMap.h"

std::shared_ptr<CMockStreetMap> CMockStreetMap::CreateTestStreetMap(){
    const char FirstStreetLetter = 'A';
    const char LastStreetLetter = 'M';
    const int StreetNumbers = 10;
    const std::string Suffix[] = {"st","nd","rd"};
    auto StreetMap = std::make_shared<CMockStreetMap>();
    // @38.5 ~54.2mi/deg ~69mi/deg
    int NodeIndex = 0;
    
    for(int StreetNumber = 0; StreetNumber < StreetNumbers; StreetNumber++){
        for(char StreetLetter = FirstStreetLetter; StreetLetter <= LastStreetLetter; StreetLetter++){
            auto TempNode = std::make_shared<SMockNode>();
            CStreetMap::TNodeID NodeID = int(StreetLetter)*10 + StreetNumber;
            EXPECT_CALL(*TempNode,ID())
                .WillRepeatedly(testing::Return(NodeID));
            EXPECT_CALL(*TempNode,Location())
                .WillRepeatedly(testing::Return(CStreetMap::SLocation{38.5 + (StreetNumber * 0.25)/69.0, -121.7 +((StreetLetter-FirstStreetLetter) * 0.25)/54.2}));
            EXPECT_CALL(*StreetMap,NodeByID(NodeID))
                .WillRepeatedly(testing::Return(TempNode));
            EXPECT_CALL(*StreetMap,NodeByIndex(NodeIndex))
                .WillRepeatedly(testing::Return(TempNode));
            NodeIndex++;
        }
    }
    {
        int StreetNumber = 0;
        char StreetLetter = LastStreetLetter + 1;
        auto TempNode = std::make_shared<SMockNode>();
        CStreetMap::TNodeID NodeID = int(LastStreetLetter + 1)*10 + StreetNumber;
        EXPECT_CALL(*TempNode,ID())
            .WillRepeatedly(testing::Return(NodeID));
        EXPECT_CALL(*TempNode,Location())
            .WillRepeatedly(testing::Return(CStreetMap::SLocation{38.5 + (StreetNumber * 0.25)/69.0, -121.7 +((StreetLetter-FirstStreetLetter) * 0.25)/54.2}));
        EXPECT_CALL(*StreetMap,NodeByID(NodeID))
            .WillRepeatedly(testing::Return(TempNode));
        EXPECT_CALL(*StreetMap,NodeByIndex(NodeIndex))
            .WillRepeatedly(testing::Return(TempNode));
        NodeIndex++;
    }

    EXPECT_CALL(*StreetMap,NodeCount())
        .WillRepeatedly(testing::Return(NodeIndex));
    std::vector<std::shared_ptr<SMockWay>> NumberedWays;
    int WayIndex = 0;
    for(int StreetNumber = 0; StreetNumber < StreetNumbers; StreetNumber++){
        auto TempWay = std::make_shared<SMockWay>();
        auto WayID = StreetNumber * 100 + 105;
        NumberedWays.push_back(TempWay);
        EXPECT_CALL(*TempWay,ID())
                .WillRepeatedly(testing::Return(WayID));
        EXPECT_CALL(*TempWay,NodeCount())
                .WillRepeatedly(testing::Return(LastStreetLetter - FirstStreetLetter + 1 + (StreetNumber == 0 ? 1 : 0)));
        for(char StreetLetter = FirstStreetLetter; StreetLetter <= LastStreetLetter; StreetLetter++){
            CStreetMap::TNodeID NodeID = int(StreetLetter)*10 + StreetNumber;
            EXPECT_CALL(*TempWay,GetNodeID(StreetLetter - FirstStreetLetter))
                .WillRepeatedly(testing::Return(NodeID));
        }
        EXPECT_CALL(*TempWay,AttributeCount())
                .WillRepeatedly(testing::Return(2));
        EXPECT_CALL(*TempWay,GetAttributeKey(0))
                .WillRepeatedly(testing::Return("name"));
        EXPECT_CALL(*TempWay,GetAttribute("name"))
                .WillRepeatedly(testing::Return(std::to_string(StreetNumber+1) + (StreetNumber < 3 ? Suffix[StreetNumber] : "th")));
        EXPECT_CALL(*TempWay,GetAttributeKey(1))
                .WillRepeatedly(testing::Return("highway"));
        EXPECT_CALL(*TempWay,GetAttribute("highway"))
                .WillRepeatedly(testing::Return(StreetNumber % 4 ? "secondary" : "primary"));
        EXPECT_CALL(*StreetMap,WayByID(WayID))
            .WillRepeatedly(testing::Return(TempWay));
        EXPECT_CALL(*StreetMap,WayByIndex(WayIndex))
            .WillRepeatedly(testing::Return(TempWay));
        WayIndex++;
    }
    {
        int StreetNumber = 0;
        char StreetLetter = LastStreetLetter + 1;
        CStreetMap::TNodeID NodeID = int(StreetLetter)*10 + StreetNumber;
        EXPECT_CALL(*NumberedWays[StreetNumber],GetNodeID(StreetLetter - FirstStreetLetter))
            .WillRepeatedly(testing::Return(NodeID));
    }
    
    for(char StreetLetter = FirstStreetLetter; StreetLetter <= LastStreetLetter; StreetLetter++){
        auto TempWay = std::make_shared<SMockWay>();
        auto WayID = (StreetLetter - FirstStreetLetter) * 1001 + 2407;
        EXPECT_CALL(*TempWay,ID())
                .WillRepeatedly(testing::Return(WayID));
        EXPECT_CALL(*TempWay,NodeCount())
                .WillRepeatedly(testing::Return(StreetNumbers));
        for(int StreetNumber = 0; StreetNumber < StreetNumbers; StreetNumber++){
            CStreetMap::TNodeID NodeID = int(StreetLetter)*10 + StreetNumber;
            EXPECT_CALL(*TempWay,GetNodeID(StreetNumber))
                .WillRepeatedly(testing::Return(NodeID));
        }
        EXPECT_CALL(*TempWay,AttributeCount())
                .WillRepeatedly(testing::Return(2));
        EXPECT_CALL(*TempWay,GetAttributeKey(0))
                .WillRepeatedly(testing::Return("name"));
        EXPECT_CALL(*TempWay,GetAttribute("name"))
                .WillRepeatedly(testing::Return(std::string(1,StreetLetter) + " St." ));
        EXPECT_CALL(*TempWay,GetAttributeKey(1))
                .WillRepeatedly(testing::Return("highway"));
        EXPECT_CALL(*TempWay,GetAttribute("highway"))
                .WillRepeatedly(testing::Return((StreetLetter - FirstStreetLetter) % 4 ? "secondary" : "primary"));
        EXPECT_CALL(*StreetMap,WayByID(WayID))
            .WillRepeatedly(testing::Return(TempWay));
        EXPECT_CALL(*StreetMap,WayByIndex(WayIndex))
            .WillRepeatedly(testing::Return(TempWay));
        WayIndex++;
    }
    
    EXPECT_CALL(*StreetMap,WayCount())
        .WillRepeatedly(testing::Return(WayIndex));

    return StreetMap;
}
