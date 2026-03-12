#include "MockBusSystem.h"

std::shared_ptr<CMockBusSystem> CMockBusSystem::CreateTestBusSystem(){
    auto BusSystem = std::make_shared<CMockBusSystem>();
    auto Stop31 = std::make_shared<SMockStop>();
    auto Stop28 = std::make_shared<SMockStop>();
    auto Stop65 = std::make_shared<SMockStop>();
    auto Stop82 = std::make_shared<SMockStop>();
    auto Stop99 = std::make_shared<SMockStop>();

    EXPECT_CALL(*BusSystem,StopCount())
        .WillRepeatedly(testing::Return(5));

    EXPECT_CALL(*Stop31,Description())
        .WillRepeatedly(testing::Return("1st & J St."));

    EXPECT_CALL(*Stop31,ID())
        .WillRepeatedly(testing::Return(31));

    EXPECT_CALL(*Stop31,NodeID())
        .WillRepeatedly(testing::Return('J'*10 + 0));

    EXPECT_CALL(*BusSystem,StopByID(31))
        .WillRepeatedly(testing::Return(Stop31));

    EXPECT_CALL(*BusSystem,StopByIndex(0))
        .WillRepeatedly(testing::Return(Stop31));

    EXPECT_CALL(*Stop28,Description())
        .WillRepeatedly(testing::Return("3rd & K St."));

    EXPECT_CALL(*Stop28,ID())
        .WillRepeatedly(testing::Return(28));

    EXPECT_CALL(*Stop28,NodeID())
        .WillRepeatedly(testing::Return('K'*10 + 2));

    EXPECT_CALL(*BusSystem,StopByID(28))
        .WillRepeatedly(testing::Return(Stop28));

    EXPECT_CALL(*BusSystem,StopByIndex(1))
        .WillRepeatedly(testing::Return(Stop28));

    EXPECT_CALL(*Stop65,Description())
        .WillRepeatedly(testing::Return("6th & F St."));

    EXPECT_CALL(*Stop65,NodeID())
        .WillRepeatedly(testing::Return('F'*10 + 5));
    
    EXPECT_CALL(*Stop65,ID())
        .WillRepeatedly(testing::Return(65));
        
    EXPECT_CALL(*BusSystem,StopByID(65))
        .WillRepeatedly(testing::Return(Stop65));

    EXPECT_CALL(*BusSystem,StopByIndex(2))
        .WillRepeatedly(testing::Return(Stop65));

    EXPECT_CALL(*Stop82,Description())
        .WillRepeatedly(testing::Return("9th & C St."));

    EXPECT_CALL(*Stop82,NodeID())
        .WillRepeatedly(testing::Return('C'*10 + 8));

    EXPECT_CALL(*Stop82,ID())
        .WillRepeatedly(testing::Return(82));
        
    EXPECT_CALL(*BusSystem,StopByID(82))
        .WillRepeatedly(testing::Return(Stop82));

    EXPECT_CALL(*BusSystem,StopByIndex(3))
        .WillRepeatedly(testing::Return(Stop82));

    std::string Description99;

    ON_CALL(*Stop99, Description())
        .WillByDefault([&Description99](){
            return Description99;
        });

    EXPECT_CALL(*Stop99, Description())
        .Times(testing::AnyNumber());

    ON_CALL(*Stop99, Description(::testing::_))
        .WillByDefault([&Description99](const std::string &value){
            return Description99 = value;
        });

    EXPECT_CALL(*Stop99, Description(::testing::_))
        .Times(testing::AnyNumber());        

    EXPECT_CALL(*Stop99,NodeID())
        .WillRepeatedly(testing::Return('N'*10 + 0));

    EXPECT_CALL(*Stop99,ID())
        .WillRepeatedly(testing::Return(99));
        
    EXPECT_CALL(*BusSystem,StopByID(99))
        .WillRepeatedly(testing::Return(Stop99));

    EXPECT_CALL(*BusSystem,StopByIndex(4))
        .WillRepeatedly(testing::Return(Stop99));

    auto RouteF = std::make_shared<SMockRoute>();
    auto RouteG = std::make_shared<SMockRoute>();

    EXPECT_CALL(*BusSystem,RouteCount())
        .WillRepeatedly(testing::Return(2));

    EXPECT_CALL(*BusSystem,RouteByName("F"))
        .WillRepeatedly(testing::Return(RouteF));

    EXPECT_CALL(*BusSystem,RouteByIndex(0))
        .WillRepeatedly(testing::Return(RouteF));

    EXPECT_CALL(*RouteF,Name())
        .WillRepeatedly(testing::Return("F"));

    EXPECT_CALL(*RouteF,StopCount())
        .WillRepeatedly(testing::Return(3));

    EXPECT_CALL(*RouteF,GetStopID(0))
        .WillRepeatedly(testing::Return(28));

    EXPECT_CALL(*RouteF,GetStopID(1))
        .WillRepeatedly(testing::Return(65));
    
    EXPECT_CALL(*RouteF,GetStopID(2))
        .WillRepeatedly(testing::Return(82));

    EXPECT_CALL(*RouteF,TripCount())
        .WillRepeatedly(testing::Return(2));

    for(auto StartHour : {8,9}){
        for(size_t StopIndex = 0; StopIndex < 3; StopIndex++){
            EXPECT_CALL(*RouteF,GetStopTime(StopIndex,StartHour-8))
                .WillRepeatedly(testing::Return(CBusSystem::TStopTime(std::chrono::hours(StartHour) + std::chrono::minutes(StopIndex*5))));
        }
    }

    EXPECT_CALL(*BusSystem,RouteByName("G"))
        .WillRepeatedly(testing::Return(RouteG));

    EXPECT_CALL(*BusSystem,RouteByIndex(1))
        .WillRepeatedly(testing::Return(RouteG));

    EXPECT_CALL(*RouteG,Name())
        .WillRepeatedly(testing::Return("G"));

    EXPECT_CALL(*RouteG,StopCount())
        .WillRepeatedly(testing::Return(3));

    EXPECT_CALL(*RouteG,GetStopID(0))
        .WillRepeatedly(testing::Return(31));

    EXPECT_CALL(*RouteG,GetStopID(1))
        .WillRepeatedly(testing::Return(28));
    
    EXPECT_CALL(*RouteG,GetStopID(2))
        .WillRepeatedly(testing::Return(82));

    EXPECT_CALL(*RouteG,TripCount())
        .WillRepeatedly(testing::Return(2));

    for(auto StartHour : {8,9}){
        for(size_t StopIndex = 0; StopIndex < 3; StopIndex++){
            EXPECT_CALL(*RouteG,GetStopTime(StopIndex,StartHour-8))
                .WillRepeatedly(testing::Return(CBusSystem::TStopTime(std::chrono::hours(StartHour) + std::chrono::minutes(20 + StopIndex*5))));
        }
    }
    
    auto Path3128 = std::make_shared<SMockPath>();
    auto Path2865 = std::make_shared<SMockPath>();
    auto Path6582 = std::make_shared<SMockPath>();
    //auto Path2882 = std::make_shared<SMockPath>();
    
    EXPECT_CALL(*BusSystem,PathByStopIDs(31,28))
        .WillRepeatedly(testing::Return(Path3128));

    EXPECT_CALL(*Path3128,StartNodeID())
        .WillRepeatedly(testing::Return('J'*10 + 0));

    EXPECT_CALL(*Path3128,EndNodeID())
        .WillRepeatedly(testing::Return('K'*10 + 2));

    EXPECT_CALL(*Path3128,NodeCount())
        .WillRepeatedly(testing::Return(4));

    EXPECT_CALL(*Path3128,GetNodeID(0))
        .WillRepeatedly(testing::Return('J'*10 + 0));

    EXPECT_CALL(*Path3128,GetNodeID(1))
        .WillRepeatedly(testing::Return('J'*10 + 1));

    EXPECT_CALL(*Path3128,GetNodeID(2))
        .WillRepeatedly(testing::Return('J'*10 + 2));

    EXPECT_CALL(*Path3128,GetNodeID(3))
        .WillRepeatedly(testing::Return('K'*10 + 2));

    EXPECT_CALL(*BusSystem,PathByStopIDs(28,65))
        .WillRepeatedly(testing::Return(Path2865));

    EXPECT_CALL(*Path2865,StartNodeID())
        .WillRepeatedly(testing::Return('K'*10 + 2));

    EXPECT_CALL(*Path2865,EndNodeID())
        .WillRepeatedly(testing::Return('F'*10 + 5));

    EXPECT_CALL(*Path2865,NodeCount())
        .WillRepeatedly(testing::Return(9));

    EXPECT_CALL(*Path2865,GetNodeID(0))
        .WillRepeatedly(testing::Return('K'*10 + 2));

    EXPECT_CALL(*Path2865,GetNodeID(1))
        .WillRepeatedly(testing::Return('K'*10 + 3));

    EXPECT_CALL(*Path2865,GetNodeID(2))
        .WillRepeatedly(testing::Return('K'*10 + 4));

    EXPECT_CALL(*Path2865,GetNodeID(3))
        .WillRepeatedly(testing::Return('K'*10 + 5));

    EXPECT_CALL(*Path2865,GetNodeID(4))
        .WillRepeatedly(testing::Return('J'*10 + 5));

    EXPECT_CALL(*Path2865,GetNodeID(5))
        .WillRepeatedly(testing::Return('I'*10 + 5));

    EXPECT_CALL(*Path2865,GetNodeID(6))
        .WillRepeatedly(testing::Return('H'*10 + 5));
    
    EXPECT_CALL(*Path2865,GetNodeID(7))
        .WillRepeatedly(testing::Return('G'*10 + 5));

    EXPECT_CALL(*Path2865,GetNodeID(8))
        .WillRepeatedly(testing::Return('F'*10 + 5));

    EXPECT_CALL(*BusSystem,PathByStopIDs(65,82))
        .WillRepeatedly(testing::Return(Path6582));

    EXPECT_CALL(*Path6582,StartNodeID())
        .WillRepeatedly(testing::Return('F'*10 + 5));

    EXPECT_CALL(*Path6582,EndNodeID())
        .WillRepeatedly(testing::Return('C'*10 + 8));

    EXPECT_CALL(*Path6582,NodeCount())
        .WillRepeatedly(testing::Return(7));

    EXPECT_CALL(*Path6582,GetNodeID(0))
        .WillRepeatedly(testing::Return('F'*10 + 5));

    EXPECT_CALL(*Path6582,GetNodeID(1))
        .WillRepeatedly(testing::Return('E'*10 + 5));

    EXPECT_CALL(*Path6582,GetNodeID(2))
        .WillRepeatedly(testing::Return('D'*10 + 5));

    EXPECT_CALL(*Path6582,GetNodeID(3))
        .WillRepeatedly(testing::Return('C'*10 + 5));

    EXPECT_CALL(*Path6582,GetNodeID(4))
        .WillRepeatedly(testing::Return('C'*10 + 6));

    EXPECT_CALL(*Path6582,GetNodeID(5))
        .WillRepeatedly(testing::Return('C'*10 + 7));

    EXPECT_CALL(*Path6582,GetNodeID(6))
        .WillRepeatedly(testing::Return('C'*10 + 8));

    return BusSystem;
}
