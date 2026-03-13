#include <gtest/gtest.h>
#include "MockBusSystem.h"
#include "BusSystemIndexer.h"

class BusSystemIndexer : public ::testing::Test{
    protected:
        std::shared_ptr<CMockBusSystem> DBusSystem;
        std::shared_ptr<CBusSystemIndexer> DBusSystemIndexer;

        void SetUp() override{
            DBusSystem = CMockBusSystem::CreateTestBusSystem();
            DBusSystemIndexer = std::make_shared<CBusSystemIndexer>(DBusSystem);
        }

        void TearDown() override{

        }
};

TEST_F(BusSystemIndexer, SimpleTest){
    std::unordered_set<CBusSystem::TStopID> StopIDs{28,31,65,82,99};
    std::unordered_set<std::string> StopDescriptions{"1st & J St.", "3rd & K St.", "6th & F St.", "9th & C St.",""};
    std::unordered_set<CStreetMap::TNodeID> StopNodeIDs{'J'*10,'K'*10+2,'F'*10+5,'C'*10+8,'N'*10+0};
    EXPECT_EQ(DBusSystemIndexer->StopCount(),5);
    EXPECT_EQ(DBusSystemIndexer->RouteCount(),2);
    auto LastID = CBusSystem::InvalidStopID;
    for(size_t Index = 0; Index < DBusSystemIndexer->StopCount(); Index++){
        auto TempStop = DBusSystemIndexer->SortedStopByIndex(Index);
        ASSERT_NE(TempStop,nullptr);
        if(LastID != CBusSystem::InvalidStopID){
            EXPECT_LT(LastID, TempStop->ID());
        }
        EXPECT_TRUE(StopIDs.contains(TempStop->ID()));
        EXPECT_TRUE(StopNodeIDs.contains(TempStop->NodeID()));
        EXPECT_TRUE(StopDescriptions.contains(TempStop->Description()));
        LastID = TempStop->ID();
    }
    EXPECT_EQ(DBusSystemIndexer->SortedStopByIndex(DBusSystemIndexer->StopCount()),nullptr);

    auto TempRoute = DBusSystemIndexer->SortedRouteByIndex(0);
    ASSERT_NE(TempRoute,nullptr);
    EXPECT_EQ(TempRoute->Name(),"F");
    EXPECT_EQ(TempRoute->StopCount(),3);
    EXPECT_EQ(TempRoute->GetStopID(0),28);
    EXPECT_EQ(TempRoute->GetStopID(1),65);
    EXPECT_EQ(TempRoute->GetStopID(2),82);
    ASSERT_EQ(TempRoute->TripCount(),2);
    EXPECT_EQ(TempRoute->GetStopTime(0,0).to_duration(),std::chrono::hours(8) + std::chrono::minutes(0));
    EXPECT_EQ(TempRoute->GetStopTime(1,0).to_duration(),std::chrono::hours(8) + std::chrono::minutes(5));
    EXPECT_EQ(TempRoute->GetStopTime(2,0).to_duration(),std::chrono::hours(8) + std::chrono::minutes(10));
    EXPECT_EQ(TempRoute->GetStopTime(0,1).to_duration(),std::chrono::hours(9) + std::chrono::minutes(0));
    EXPECT_EQ(TempRoute->GetStopTime(1,1).to_duration(),std::chrono::hours(9) + std::chrono::minutes(5));
    EXPECT_EQ(TempRoute->GetStopTime(2,1).to_duration(),std::chrono::hours(9) + std::chrono::minutes(10));

    TempRoute = DBusSystemIndexer->SortedRouteByIndex(1);
    ASSERT_NE(TempRoute,nullptr);
    EXPECT_EQ(TempRoute->Name(),"G");
    EXPECT_EQ(TempRoute->StopCount(),3);
    EXPECT_EQ(TempRoute->GetStopID(0),31);
    EXPECT_EQ(TempRoute->GetStopID(1),28);
    EXPECT_EQ(TempRoute->GetStopID(2),82);
    ASSERT_EQ(TempRoute->TripCount(),2);
    EXPECT_EQ(TempRoute->GetStopTime(0,0).to_duration(),std::chrono::hours(8) + std::chrono::minutes(20));
    EXPECT_EQ(TempRoute->GetStopTime(1,0).to_duration(),std::chrono::hours(8) + std::chrono::minutes(25));
    EXPECT_EQ(TempRoute->GetStopTime(2,0).to_duration(),std::chrono::hours(8) + std::chrono::minutes(30));
    EXPECT_EQ(TempRoute->GetStopTime(0,1).to_duration(),std::chrono::hours(9) + std::chrono::minutes(20));
    EXPECT_EQ(TempRoute->GetStopTime(1,1).to_duration(),std::chrono::hours(9) + std::chrono::minutes(25));
    EXPECT_EQ(TempRoute->GetStopTime(2,1).to_duration(),std::chrono::hours(9) + std::chrono::minutes(30));

    EXPECT_EQ(DBusSystemIndexer->SortedRouteByIndex(2),nullptr);
    EXPECT_EQ(DBusSystemIndexer->RouteByName("FOO"),nullptr);
}

TEST_F(BusSystemIndexer, RouteByStop){
    EXPECT_EQ(DBusSystemIndexer->StopCount(),5);
    EXPECT_EQ(DBusSystemIndexer->RouteCount(),2);
    std::unordered_set<std::string> Routes;
    EXPECT_TRUE(DBusSystemIndexer->RoutesByStopIDs(28,82,Routes));
    EXPECT_NE(Routes.find("F"),Routes.end());
    EXPECT_NE(Routes.find("G"),Routes.end());
    EXPECT_FALSE(DBusSystemIndexer->RoutesByStopIDs(1,33,Routes));
    EXPECT_TRUE(DBusSystemIndexer->RoutesByStopID(82,Routes));
    EXPECT_NE(Routes.find("F"),Routes.end());
    EXPECT_NE(Routes.find("G"),Routes.end());
    EXPECT_FALSE(DBusSystemIndexer->RoutesByStopID(44,Routes));
}

TEST_F(BusSystemIndexer, StopsByRoutes){
    EXPECT_EQ(DBusSystemIndexer->StopCount(),5);
    EXPECT_EQ(DBusSystemIndexer->RouteCount(),2);
    std::unordered_set<CBusSystemIndexer::TStopID> Stops;
    EXPECT_TRUE(DBusSystemIndexer->StopIDsByRoutes("F","G",Stops));
    EXPECT_TRUE(Stops.contains(28));
    EXPECT_TRUE(Stops.contains(82));
    EXPECT_TRUE(DBusSystemIndexer->StopIDsByRoutes("G","F",Stops));
    EXPECT_TRUE(Stops.contains(28));
    EXPECT_TRUE(Stops.contains(82));
    EXPECT_FALSE(DBusSystemIndexer->StopIDsByRoutes("C","A",Stops));
}

TEST_F(BusSystemIndexer, RouteByNameSuccess){
    auto RouteF = DBusSystemIndexer->RouteByName("F");
    ASSERT_NE(RouteF, nullptr);
    EXPECT_EQ(RouteF->Name(), "F");
    EXPECT_EQ(RouteF->StopCount(), 3);
    EXPECT_EQ(RouteF->TripCount(), 2);
    EXPECT_EQ(RouteF->GetStopID(0), 28);
    EXPECT_EQ(RouteF->GetStopID(1), 65);
    EXPECT_EQ(RouteF->GetStopID(2), 82);

    auto RouteG = DBusSystemIndexer->RouteByName("G");
    ASSERT_NE(RouteG, nullptr);
    EXPECT_EQ(RouteG->Name(), "G");
    EXPECT_EQ(RouteG->StopCount(), 3);
    EXPECT_EQ(RouteG->TripCount(), 2);
    EXPECT_EQ(RouteG->GetStopID(0), 31);
    EXPECT_EQ(RouteG->GetStopID(1), 28);
    EXPECT_EQ(RouteG->GetStopID(2), 82);
}

TEST_F(BusSystemIndexer, RouteIndexerFindStopIndex){
    auto RouteF = DBusSystemIndexer->RouteByName("F");
    ASSERT_NE(RouteF, nullptr);

    EXPECT_EQ(RouteF->FindStopIndex(28), 0);
    EXPECT_EQ(RouteF->FindStopIndex(65), 1);
    EXPECT_EQ(RouteF->FindStopIndex(82), 2);
    EXPECT_EQ(RouteF->FindStopIndex(31), RouteF->StopCount());
    EXPECT_EQ(RouteF->FindStopIndex(999), RouteF->StopCount());
}

TEST_F(BusSystemIndexer, RouteIndexerFindStopIndexWithStart){
    auto RouteG = DBusSystemIndexer->RouteByName("G");
    ASSERT_NE(RouteG, nullptr);

    EXPECT_EQ(RouteG->FindStopIndex(31, 0), 0);
    EXPECT_EQ(RouteG->FindStopIndex(28, 0), 1);
    EXPECT_EQ(RouteG->FindStopIndex(82, 1), 2);
    EXPECT_EQ(RouteG->FindStopIndex(31, 1), RouteG->StopCount());
    EXPECT_EQ(RouteG->FindStopIndex(82, RouteG->StopCount()), RouteG->StopCount());
    EXPECT_EQ(RouteG->FindStopIndex(82, RouteG->StopCount() + 5), RouteG->StopCount());
}

TEST_F(BusSystemIndexer, RouteIndexerStopIDsSourceDestinationForward){
    auto RouteF = DBusSystemIndexer->RouteByName("F");
    ASSERT_NE(RouteF, nullptr);

    auto Stops = RouteF->StopIDsSourceDestination(28, 82);
    ASSERT_EQ(Stops.size(), 3);
    EXPECT_EQ(Stops[0], 28);
    EXPECT_EQ(Stops[1], 65);
    EXPECT_EQ(Stops[2], 82);

    Stops = RouteF->StopIDsSourceDestination(65, 82);
    ASSERT_EQ(Stops.size(), 2);
    EXPECT_EQ(Stops[0], 65);
    EXPECT_EQ(Stops[1], 82);

    Stops = RouteF->StopIDsSourceDestination(65, 65);
    ASSERT_EQ(Stops.size(), 1);
    EXPECT_EQ(Stops[0], 65);
}

TEST_F(BusSystemIndexer, RouteIndexerStopIDsSourceDestinationReverse){
    auto RouteF = DBusSystemIndexer->RouteByName("F");
    ASSERT_NE(RouteF, nullptr);

    auto Stops = RouteF->StopIDsSourceDestination(82, 28);
    ASSERT_EQ(Stops.size(), 3);
    EXPECT_EQ(Stops[0], 82);
    EXPECT_EQ(Stops[1], 65);
    EXPECT_EQ(Stops[2], 28);

    Stops = RouteF->StopIDsSourceDestination(82, 65);
    ASSERT_EQ(Stops.size(), 2);
    EXPECT_EQ(Stops[0], 82);
    EXPECT_EQ(Stops[1], 65);
}

TEST_F(BusSystemIndexer, RouteIndexerStopIDsSourceDestinationMissingStops){
    auto RouteG = DBusSystemIndexer->RouteByName("G");
    ASSERT_NE(RouteG, nullptr);

    auto Stops = RouteG->StopIDsSourceDestination(999, 82);
    EXPECT_TRUE(Stops.empty());

    Stops = RouteG->StopIDsSourceDestination(31, 999);
    EXPECT_TRUE(Stops.empty());

    Stops = RouteG->StopIDsSourceDestination(65, 82);
    EXPECT_TRUE(Stops.empty());
}

TEST_F(BusSystemIndexer, RoutesByStopIDClearsOutputSet){
    std::unordered_set<std::string> Routes{"OLD", "DATA"};

    EXPECT_TRUE(DBusSystemIndexer->RoutesByStopID(28, Routes));
    EXPECT_EQ(Routes.size(), 2);
    EXPECT_TRUE(Routes.contains("F"));
    EXPECT_TRUE(Routes.contains("G"));
    EXPECT_FALSE(Routes.contains("OLD"));
    EXPECT_FALSE(Routes.contains("DATA"));

    EXPECT_FALSE(DBusSystemIndexer->RoutesByStopID(12345, Routes));
    EXPECT_TRUE(Routes.empty());
}

TEST_F(BusSystemIndexer, RoutesByStopIDsClearsOutputSetAndFindsIntersection){
    std::unordered_set<std::string> Routes{"SHOULD", "CLEAR"};

    EXPECT_TRUE(DBusSystemIndexer->RoutesByStopIDs(28, 82, Routes));
    EXPECT_EQ(Routes.size(), 2);
    EXPECT_TRUE(Routes.contains("F"));
    EXPECT_TRUE(Routes.contains("G"));
    EXPECT_FALSE(Routes.contains("SHOULD"));
    EXPECT_FALSE(Routes.contains("CLEAR"));

    EXPECT_FALSE(DBusSystemIndexer->RoutesByStopIDs(31, 65, Routes));
    EXPECT_TRUE(Routes.empty());
}

TEST_F(BusSystemIndexer, StopIDsByRoutesClearsOutputSet){
    std::unordered_set<CBusSystemIndexer::TStopID> Stops{1, 2, 3};

    EXPECT_TRUE(DBusSystemIndexer->StopIDsByRoutes("F", "G", Stops));
    EXPECT_EQ(Stops.size(), 2);
    EXPECT_TRUE(Stops.contains(28));
    EXPECT_TRUE(Stops.contains(82));
    EXPECT_FALSE(Stops.contains(1));
    EXPECT_FALSE(Stops.contains(2));
    EXPECT_FALSE(Stops.contains(3));

    EXPECT_FALSE(DBusSystemIndexer->StopIDsByRoutes("F", "BADROUTE", Stops));
    EXPECT_TRUE(Stops.empty());
}

TEST_F(BusSystemIndexer, StopIDsByRoutesSameRoute){
    std::unordered_set<CBusSystemIndexer::TStopID> Stops;

    EXPECT_TRUE(DBusSystemIndexer->StopIDsByRoutes("F", "F", Stops));
    EXPECT_EQ(Stops.size(), 3);
    EXPECT_TRUE(Stops.contains(28));
    EXPECT_TRUE(Stops.contains(65));
    EXPECT_TRUE(Stops.contains(82));

    EXPECT_TRUE(DBusSystemIndexer->StopIDsByRoutes("G", "G", Stops));
    EXPECT_EQ(Stops.size(), 3);
    EXPECT_TRUE(Stops.contains(31));
    EXPECT_TRUE(Stops.contains(28));
    EXPECT_TRUE(Stops.contains(82));
}