#include <gtest/gtest.h>
#include "StringDataSource.h"
#include "XMLReader.h"
#include "XMLBusSystem.h"
#include "MockBusSystem.h"
#include "TripPlanner.h"

#include <iostream>
using std::cout;
using std::endl;

class TripPlanner : public ::testing::Test{
    protected:
        std::shared_ptr<CBusSystem> DBusSystem;
        std::shared_ptr<CTripPlanner> DTripPlanner;

        void SetUp() override{
            DBusSystem = CMockBusSystem::CreateTestBusSystem();
            DTripPlanner = std::make_shared<CTripPlanner>(DBusSystem);
        }

        void TearDown() override{
            
        }

};

TEST_F(TripPlanner, DirectRouteTest){
    auto Route = DTripPlanner->FindDirectRouteLeaveTime(28,65,CTripPlanner::TStopTime(std::chrono::hours(8)));
    ASSERT_NE(Route,nullptr);
    EXPECT_EQ(Route->Name(),"F");
    Route = DTripPlanner->FindDirectRouteArrivalTime(28,82,CTripPlanner::TStopTime(std::chrono::hours(8)+std::chrono::minutes(46)));
    ASSERT_NE(Route,nullptr);
    EXPECT_EQ(Route->Name(),"G");
}

TEST_F(TripPlanner, FindRouteTest){
    CTripPlanner::TTravelPlan TravelPlan;
    EXPECT_TRUE(DTripPlanner->FindRouteLeaveTime(28,65,CTripPlanner::TStopTime(std::chrono::hours(8)),TravelPlan));
    ASSERT_EQ(TravelPlan.size(),2);
    EXPECT_EQ(TravelPlan[0].DRouteName,"F");
    EXPECT_EQ(TravelPlan[0].DStopID,28);
    EXPECT_EQ(TravelPlan[0].DTime.to_duration(),CTripPlanner::TStopTime(std::chrono::hours(8)).to_duration());
    EXPECT_EQ(TravelPlan[1].DRouteName,"");
    EXPECT_EQ(TravelPlan[1].DStopID,65);
    EXPECT_EQ(TravelPlan[1].DTime.to_duration(),CTripPlanner::TStopTime(std::chrono::hours(8) + std::chrono::minutes(5)).to_duration());


    EXPECT_TRUE(DTripPlanner->FindRouteArrivalTime(28,82,CTripPlanner::TStopTime(std::chrono::hours(8)+std::chrono::minutes(46)),TravelPlan));
    ASSERT_EQ(TravelPlan.size(),2);
    EXPECT_EQ(TravelPlan[0].DRouteName,"G");
    EXPECT_EQ(TravelPlan[0].DStopID,28);
    EXPECT_EQ(TravelPlan[0].DTime.to_duration(),CTripPlanner::TStopTime(std::chrono::hours(8) + std::chrono::minutes(25)).to_duration());
    EXPECT_EQ(TravelPlan[1].DRouteName,"");
    EXPECT_EQ(TravelPlan[1].DStopID,82);
    EXPECT_EQ(TravelPlan[1].DTime.to_duration(),CTripPlanner::TStopTime(std::chrono::hours(8) + std::chrono::minutes(30)).to_duration());
}

TEST_F(TripPlanner, FindOneStopRouteTest){
    CTripPlanner::TTravelPlan TravelPlan;
    EXPECT_TRUE(DTripPlanner->FindRouteLeaveTime(31,65,CTripPlanner::TStopTime(std::chrono::hours(8) + std::chrono::minutes(10)),TravelPlan));
    ASSERT_EQ(TravelPlan.size(),3);
    EXPECT_EQ(TravelPlan[0].DRouteName,"G");
    EXPECT_EQ(TravelPlan[0].DStopID,31);
    EXPECT_EQ(TravelPlan[0].DTime.to_duration(),CTripPlanner::TStopTime(std::chrono::hours(8) + std::chrono::minutes(20)).to_duration());
    EXPECT_EQ(TravelPlan[1].DRouteName,"F");
    EXPECT_EQ(TravelPlan[1].DStopID,28);
    EXPECT_EQ(TravelPlan[1].DTime.to_duration(),CTripPlanner::TStopTime(std::chrono::hours(9) + std::chrono::minutes(0)).to_duration());
    EXPECT_EQ(TravelPlan[2].DRouteName,"");
    EXPECT_EQ(TravelPlan[2].DStopID,65);
    EXPECT_EQ(TravelPlan[2].DTime.to_duration(),CTripPlanner::TStopTime(std::chrono::hours(9) + std::chrono::minutes(5)).to_duration());

    EXPECT_TRUE(DTripPlanner->FindRouteArrivalTime(31,65,CTripPlanner::TStopTime(std::chrono::hours(9) + std::chrono::minutes(10)),TravelPlan));
    ASSERT_EQ(TravelPlan.size(),3);
    EXPECT_EQ(TravelPlan[0].DRouteName,"G");
    EXPECT_EQ(TravelPlan[0].DStopID,31);
    EXPECT_EQ(TravelPlan[0].DTime.to_duration(),CTripPlanner::TStopTime(std::chrono::hours(8) + std::chrono::minutes(20)).to_duration());
    EXPECT_EQ(TravelPlan[1].DRouteName,"F");
    EXPECT_EQ(TravelPlan[1].DStopID,28);
    EXPECT_EQ(TravelPlan[1].DTime.to_duration(),CTripPlanner::TStopTime(std::chrono::hours(9) + std::chrono::minutes(0)).to_duration());
    EXPECT_EQ(TravelPlan[2].DRouteName,"");
    EXPECT_EQ(TravelPlan[2].DStopID,65);
    EXPECT_EQ(TravelPlan[2].DTime.to_duration(),CTripPlanner::TStopTime(std::chrono::hours(9) + std::chrono::minutes(5)).to_duration());

    EXPECT_FALSE(DTripPlanner->FindRouteLeaveTime(31,65,CTripPlanner::TStopTime(std::chrono::hours(10) + std::chrono::minutes(10)),TravelPlan));
    EXPECT_FALSE(DTripPlanner->FindRouteArrivalTime(31,65,CTripPlanner::TStopTime(std::chrono::hours(6) + std::chrono::minutes(35)),TravelPlan));
}