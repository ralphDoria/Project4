#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "MockBusSystem.h"
#include "StringDataSink.h"
#include "TextTripPlanWriter.h"

#include <iostream>
using std::cout;
using std::endl;

class TextTripPlanWriter : public ::testing::Test{
    protected:
        std::shared_ptr<CMockBusSystem> DBusSystem;
        std::shared_ptr<CStringDataSink> DDataSink;
        std::shared_ptr<CTextTripPlanWriter> DPlanWriter;
        void SetUp() override{
            DBusSystem = CMockBusSystem::CreateTestBusSystem();
            DDataSink = std::make_shared<CStringDataSink>();
            DPlanWriter = std::make_shared<CTextTripPlanWriter>(DBusSystem);
        }

        void TearDown() override{
            
        }

};



TEST_F(TextTripPlanWriter, SimpleRoute){
    CTripPlanner::TTravelPlan TravelPlan;
    TravelPlan.push_back({CBusSystem::TStopTime(std::chrono::hours(8)),28,"F"});
    TravelPlan.push_back({CBusSystem::TStopTime(std::chrono::hours(8) + std::chrono::minutes(10)),82,""});

    EXPECT_TRUE(DPlanWriter->WritePlan(DDataSink,TravelPlan));
    auto PlanString = DDataSink->String();
    EXPECT_EQ(PlanString," 8:00 AM: Take the F bus from 3rd & K St. (stop 28).\n"
                         " 8:10 AM: Get off the F bus at 9th & C St. (stop 82).\n");
}


TEST_F(TextTripPlanWriter, OneStopRoute){
    CTripPlanner::TTravelPlan TravelPlan;
    TravelPlan.push_back({CBusSystem::TStopTime(std::chrono::hours(8)),31,"G"});
    TravelPlan.push_back({CBusSystem::TStopTime(std::chrono::hours(8) + std::chrono::minutes(15)),28,"F"});
    TravelPlan.push_back({CBusSystem::TStopTime(std::chrono::hours(8) + std::chrono::minutes(25)),82,""});

    EXPECT_TRUE(DPlanWriter->WritePlan(DDataSink,TravelPlan));
    auto PlanString = DDataSink->String();
    EXPECT_EQ(PlanString," 8:00 AM: Take the G bus from 1st & J St. (stop 31).\n"
                         "        : Get off the G bus at 3rd & K St. (stop 28) and wait for the F bus.\n"
                         " 8:15 AM: Take the F bus from 3rd & K St. (stop 28).\n"
                         " 8:25 AM: Get off the F bus at 9th & C St. (stop 82).\n");
}

TEST_F(TextTripPlanWriter, ConfigTest){
    auto Config = DPlanWriter->Config();
    Config->DisableFlag("foo");
    Config->EnableFlag("foo");
    EXPECT_FALSE(Config->FlagEnabled("foo"));
    Config->SetOption("bar",1);
    Config->SetOption("bar",3.5);
    Config->SetOption("bar","foo");
    EXPECT_FALSE(Config->GetOption("bar").has_value());
    EXPECT_EQ(Config->GetOptionType("bar"),CTripPlanWriter::SConfig::EOptionType::None);
    Config->ClearOption("");
}

TEST_F(TextTripPlanWriter, VerboseOutput){
    auto Config = DPlanWriter->Config();
    EXPECT_FALSE(Config->FlagEnabled(CTextTripPlanWriter::Verbose));
    Config->EnableFlag(CTextTripPlanWriter::Verbose);
    EXPECT_TRUE(Config->FlagEnabled(CTextTripPlanWriter::Verbose));

    CTripPlanner::TTravelPlan TravelPlan;
    TravelPlan.push_back({CBusSystem::TStopTime(std::chrono::hours(8)),28,"F"});
    TravelPlan.push_back({CBusSystem::TStopTime(std::chrono::hours(8) + std::chrono::minutes(10)),82,""});

    EXPECT_TRUE(DPlanWriter->WritePlan(DDataSink,TravelPlan));
    auto PlanString = DDataSink->String();
    EXPECT_EQ(PlanString," 8:00 AM: Take the F bus from 3rd & K St. (stop 28).\n"
                         " 8:05 AM: Stay on the F bus at 6th & F St. (stop 65).\n"
                         " 8:10 AM: Get off the F bus at 9th & C St. (stop 82).\n");


    Config->DisableFlag(CTextTripPlanWriter::Verbose);
    EXPECT_FALSE(Config->FlagEnabled(CTextTripPlanWriter::Verbose));
}