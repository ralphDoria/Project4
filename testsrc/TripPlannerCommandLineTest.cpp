#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "MockBusSystem.h"
#include "MockStreetMap.h"
#include "HTMLTripPlanWriter.h"
#include "TextTripPlanWriter.h"
#include "TripPlannerCommandLine.h"
#include "StringDataSink.h"
#include "StringDataSource.h"

#include <iostream>
using std::cout;
using std::endl;

class CMockFactory : public CDataFactory{
    public:
        MOCK_METHOD(std::shared_ptr< CDataSource >, CreateSource, (const std::string &name), (noexcept, override));
        MOCK_METHOD(std::shared_ptr< CDataSink >, CreateSink, (const std::string &name), (noexcept, override));
};

class TripPlannerCommandLine : public ::testing::Test{
    protected:
        std::shared_ptr<CMockBusSystem> DBusSystem;
        std::shared_ptr<CMockStreetMap> DStreetMap;
        std::shared_ptr<CStringDataSink> DDataOutSink;
        std::shared_ptr<CStringDataSink> DDataErrSink;
        std::shared_ptr<CMockFactory> DMockFactory;
        std::shared_ptr<CTripPlanner> DTripPlanner;
        std::shared_ptr<CTripPlanWriter> DOutTripPlanWriter;
        std::shared_ptr<CTripPlanWriter> DStorageTripPlanWriter;

        void SetUp() override{
            DBusSystem = CMockBusSystem::CreateTestBusSystem();
            DStreetMap = CMockStreetMap::CreateTestStreetMap();
            DDataOutSink = std::make_shared<CStringDataSink>();
            DDataErrSink = std::make_shared<CStringDataSink>();
            DTripPlanner = std::make_shared<CTripPlanner>(DBusSystem);
            DOutTripPlanWriter = std::make_shared<CTextTripPlanWriter>(DBusSystem);
            DStorageTripPlanWriter = std::make_shared<CHTMLTripPlanWriter>(DStreetMap,DBusSystem);
            DMockFactory = std::make_shared<CMockFactory>();
        
        }

        void TearDown() override{
            
        }
};


TEST_F(TripPlannerCommandLine, SimpleTest){
    auto InputSource = std::make_shared<CStringDataSource>("exit\n");
    auto Config = std::make_shared<CTripPlannerCommandLine::SConfig>(InputSource,
                                                                        DDataOutSink,
                                                                        DDataErrSink,
                                                                        DMockFactory,
                                                                        DTripPlanner,
                                                                        DStreetMap,
                                                                        DOutTripPlanWriter,
                                                                        DStorageTripPlanWriter);
    CTripPlannerCommandLine CommandLine(Config);

    EXPECT_TRUE(CommandLine.ProcessCommands());
    EXPECT_EQ(DDataOutSink->String(),"> ");
    EXPECT_TRUE(DDataErrSink->String().empty());
}

TEST_F(TripPlannerCommandLine, HelpTest){
    auto InputSource = std::make_shared<CStringDataSource>( "help\n"
                                                            "exit\n");
    auto Config = std::make_shared<CTripPlannerCommandLine::SConfig>(InputSource,
                                                                        DDataOutSink,
                                                                        DDataErrSink,
                                                                        DMockFactory,
                                                                        DTripPlanner,
                                                                        DStreetMap,
                                                                        DOutTripPlanWriter,
                                                                        DStorageTripPlanWriter);
    CTripPlannerCommandLine CommandLine(Config);

    EXPECT_TRUE(CommandLine.ProcessCommands());
    EXPECT_EQ(DDataOutSink->String(),"> "
                                    "--------------------------------------------------------------------------\n"
                                    "help     Display this help menu\n"
                                    "exit     Exit the program\n"
                                    "count    Output the number of stops in the system\n"
                                    "config   Output the current configuration\n"
                                    "toggle   Syntax \"toggle flag\"\n"
                                    "         Will toggle the flag specified.\n"
                                    "set      Syntax \"set option value\"\n"
                                    "         Will set the option specified with the value.\n"
                                    "stop     Syntax \"stop [0, count)\"\n"
                                    "         Will output stop ID, node ID, and Lat/Lon for and description.\n"
                                    "leaveat  Syntax \"leaveat time start end\" \n"
                                    "         Calculates the best trip plan from start to end leaving at time.\n"
                                    "arriveby Syntax \"arriveby time start end\" \n"
                                    "         Calculates the best trip plan from start to end arriving by time.\n"
                                    "save     Saves the last calculated trip to file\n"
                                    "> ");
    EXPECT_TRUE(DDataErrSink->String().empty());
}

TEST_F(TripPlannerCommandLine, CountTest){
    auto InputSource = std::make_shared<CStringDataSource>( "count\n"
                                                            "exit\n");
    auto Config = std::make_shared<CTripPlannerCommandLine::SConfig>(InputSource,
                                                                        DDataOutSink,
                                                                        DDataErrSink,
                                                                        DMockFactory,
                                                                        DTripPlanner,
                                                                        DStreetMap,
                                                                        DOutTripPlanWriter,
                                                                        DStorageTripPlanWriter);
    CTripPlannerCommandLine CommandLine(Config);

    EXPECT_TRUE(CommandLine.ProcessCommands());
    EXPECT_EQ(DDataOutSink->String(),"> "
                                    "5 stops\n"
                                    "> ");
    EXPECT_TRUE(DDataErrSink->String().empty());
}

TEST_F(TripPlannerCommandLine, ConfigTest){
    auto InputSource = std::make_shared<CStringDataSource>( "config\n"
                                                            "exit\n");
    auto Config = std::make_shared<CTripPlannerCommandLine::SConfig>(InputSource,
                                                                        DDataOutSink,
                                                                        DDataErrSink,
                                                                        DMockFactory,
                                                                        DTripPlanner,
                                                                        DStreetMap,
                                                                        DOutTripPlanWriter,
                                                                        DStorageTripPlanWriter);
    CTripPlannerCommandLine CommandLine(Config);

    EXPECT_TRUE(CommandLine.ProcessCommands());
    EXPECT_EQ(DDataOutSink->String(),"> "
                                     "motorway-enabled   : true\n"
                                     "primary-enabled    : true\n"
                                     "residential-enabled: true\n"
                                     "secondary-enabled  : true\n"
                                     "tertiary-enabled   : true\n"
                                     "verbose            : false\n"
                                     "bus-color-0        : #8E24AA\n"
                                     "bus-color-1        : #F57C00\n"
                                     "bus-stroke         : 8\n"
                                     "busstop-radius     : 8.000000\n"
                                     "destination-color  : #FF0000\n"
                                     "destination-radius : 8.000000\n"
                                     "label-background   : #FFFFFF80\n"
                                     "label-color        : #000000\n"
                                     "label-margin       : 8\n"
                                     "label-paint-order  : stroke fill\n"
                                     "label-size         : 16\n"
                                     "motorway-stroke    : 6\n"
                                     "primary-stroke     : 4\n"
                                     "residential-stroke : 2\n"
                                     "secondary-stroke   : 2\n"
                                     "source-color       : #00FF00\n"
                                     "source-radius      : 8.000000\n"
                                     "street-color       : #B0B0B0\n"
                                     "svg-height         : 540\n"
                                     "svg-margin         : 30\n"
                                     "svg-width          : 960\n"
                                     "tertiary-stroke    : 2\n"
                                     "> ");
    EXPECT_TRUE(DDataErrSink->String().empty());
}

TEST_F(TripPlannerCommandLine, ToggleTest){
    auto InputSource = std::make_shared<CStringDataSource>( "toggle residential-enabled\n"
                                                            "toggle tertiary-enabled\n"
                                                            "toggle primary-enabled\n"
                                                            "toggle primary-enabled\n"
                                                            "config\n"
                                                            "exit\n");
    auto Config = std::make_shared<CTripPlannerCommandLine::SConfig>(InputSource,
                                                                        DDataOutSink,
                                                                        DDataErrSink,
                                                                        DMockFactory,
                                                                        DTripPlanner,
                                                                        DStreetMap,
                                                                        DOutTripPlanWriter,
                                                                        DStorageTripPlanWriter);
    CTripPlannerCommandLine CommandLine(Config);

    EXPECT_TRUE(CommandLine.ProcessCommands());
    EXPECT_EQ(DDataOutSink->String(),"> Flag residential-enabled is now false\n"
                                     "> Flag tertiary-enabled is now false\n"
                                     "> Flag primary-enabled is now false\n"
                                     "> Flag primary-enabled is now true\n"
                                     "> "
                                     "motorway-enabled   : true\n"
                                     "primary-enabled    : true\n"
                                     "residential-enabled: false\n"
                                     "secondary-enabled  : true\n"
                                     "tertiary-enabled   : false\n"
                                     "verbose            : false\n"
                                     "bus-color-0        : #8E24AA\n"
                                     "bus-color-1        : #F57C00\n"
                                     "bus-stroke         : 8\n"
                                     "busstop-radius     : 8.000000\n"
                                     "destination-color  : #FF0000\n"
                                     "destination-radius : 8.000000\n"
                                     "label-background   : #FFFFFF80\n"
                                     "label-color        : #000000\n"
                                     "label-margin       : 8\n"
                                     "label-paint-order  : stroke fill\n"
                                     "label-size         : 16\n"
                                     "motorway-stroke    : 6\n"
                                     "primary-stroke     : 4\n"
                                     "residential-stroke : 2\n"
                                     "secondary-stroke   : 2\n"
                                     "source-color       : #00FF00\n"
                                     "source-radius      : 8.000000\n"
                                     "street-color       : #B0B0B0\n"
                                     "svg-height         : 540\n"
                                     "svg-margin         : 30\n"
                                     "svg-width          : 960\n"
                                     "tertiary-stroke    : 2\n"
                                     "> ");
    EXPECT_TRUE(DDataErrSink->String().empty());
}

TEST_F(TripPlannerCommandLine, SetTest){
    auto InputSource = std::make_shared<CStringDataSource>( "set label-size 14\n"
                                                            "set destination-radius 7.0\n"
                                                            "set bus-color-0 #00FFAA\n"
                                                            "config\n"
                                                            "exit\n");
    auto Config = std::make_shared<CTripPlannerCommandLine::SConfig>(InputSource,
                                                                        DDataOutSink,
                                                                        DDataErrSink,
                                                                        DMockFactory,
                                                                        DTripPlanner,
                                                                        DStreetMap,
                                                                        DOutTripPlanWriter,
                                                                        DStorageTripPlanWriter);
    CTripPlannerCommandLine CommandLine(Config);

    EXPECT_TRUE(CommandLine.ProcessCommands());
    EXPECT_EQ(DDataOutSink->String(),"> Option label-size is now 14\n"
                                     "> Option destination-radius is now 7.000000\n"
                                     "> Option bus-color-0 is now #00FFAA\n"
                                     "> "
                                     "motorway-enabled   : true\n"
                                     "primary-enabled    : true\n"
                                     "residential-enabled: true\n"
                                     "secondary-enabled  : true\n"
                                     "tertiary-enabled   : true\n"
                                     "verbose            : false\n"
                                     "bus-color-0        : #00FFAA\n"
                                     "bus-color-1        : #F57C00\n"
                                     "bus-stroke         : 8\n"
                                     "busstop-radius     : 8.000000\n"
                                     "destination-color  : #FF0000\n"
                                     "destination-radius : 7.000000\n"
                                     "label-background   : #FFFFFF80\n"
                                     "label-color        : #000000\n"
                                     "label-margin       : 8\n"
                                     "label-paint-order  : stroke fill\n"
                                     "label-size         : 14\n"
                                     "motorway-stroke    : 6\n"
                                     "primary-stroke     : 4\n"
                                     "residential-stroke : 2\n"
                                     "secondary-stroke   : 2\n"
                                     "source-color       : #00FF00\n"
                                     "source-radius      : 8.000000\n"
                                     "street-color       : #B0B0B0\n"
                                     "svg-height         : 540\n"
                                     "svg-margin         : 30\n"
                                     "svg-width          : 960\n"
                                     "tertiary-stroke    : 2\n"
                                     "> ");
    EXPECT_TRUE(DDataErrSink->String().empty());
}

TEST_F(TripPlannerCommandLine, StopTest){
    auto InputSource = std::make_shared<CStringDataSource>("stop 0\nexit\n");
    auto Config = std::make_shared<CTripPlannerCommandLine::SConfig>(InputSource,
                                                                        DDataOutSink,
                                                                        DDataErrSink,
                                                                        DMockFactory,
                                                                        DTripPlanner,
                                                                        DStreetMap,
                                                                        DOutTripPlanWriter,
                                                                        DStorageTripPlanWriter);
    CTripPlannerCommandLine CommandLine(Config);

    EXPECT_TRUE(CommandLine.ProcessCommands());
    EXPECT_EQ(DDataOutSink->String(),"> "
                                    "Stop 0:\n"
                                    "    ID          : 28\n"
                                    "    Node ID     : 752\n"
                                    "    Location    : 38d 30' 26.09\" N, 121d 39' 13.95\" W\n"
                                    "    Description : 3rd & K St.\n"
                                    "> ");
    EXPECT_TRUE(DDataErrSink->String().empty());
}

TEST_F(TripPlannerCommandLine, LeaveAt){
    auto InputSource = std::make_shared<CStringDataSource>( "leaveat 8:00AM 28 82\n"
                                                            "exit\n");
    auto Config = std::make_shared<CTripPlannerCommandLine::SConfig>(InputSource,
                                                                        DDataOutSink,
                                                                        DDataErrSink,
                                                                        DMockFactory,
                                                                        DTripPlanner,
                                                                        DStreetMap,
                                                                        DOutTripPlanWriter,
                                                                        DStorageTripPlanWriter);
    CTripPlannerCommandLine CommandLine(Config);

    EXPECT_TRUE(CommandLine.ProcessCommands());
    EXPECT_EQ(DDataOutSink->String(),"> "
                                    " 8:00 AM: Take the F bus from 3rd & K St. (stop 28).\n"
                                    " 8:10 AM: Get off the F bus at 9th & C St. (stop 82).\n"
                                    "> ");
    EXPECT_TRUE(DDataErrSink->String().empty());
}

TEST_F(TripPlannerCommandLine, ArriveBy){
    auto InputSource = std::make_shared<CStringDataSource>( "arriveby 8:46AM 28 82\n"
                                                            "exit\n");
    auto Config = std::make_shared<CTripPlannerCommandLine::SConfig>(InputSource,
                                                                        DDataOutSink,
                                                                        DDataErrSink,
                                                                        DMockFactory,
                                                                        DTripPlanner,
                                                                        DStreetMap,
                                                                        DOutTripPlanWriter,
                                                                        DStorageTripPlanWriter);
    CTripPlannerCommandLine CommandLine(Config);

    EXPECT_TRUE(CommandLine.ProcessCommands());
    EXPECT_EQ(DDataOutSink->String(),"> "
                                    " 8:25 AM: Take the G bus from 3rd & K St. (stop 28).\n"
                                    " 8:30 AM: Get off the G bus at 9th & C St. (stop 82).\n"
                                    "> ");
    EXPECT_TRUE(DDataErrSink->String().empty());
}

TEST_F(TripPlannerCommandLine, SaveTest){
    auto InputSource = std::make_shared<CStringDataSource>( "leaveat 8:00AM 28 82\n"
                                                            "save\n"
                                                            "exit\n");
    auto Config = std::make_shared<CTripPlannerCommandLine::SConfig>(InputSource,
                                                                        DDataOutSink,
                                                                        DDataErrSink,
                                                                        DMockFactory,
                                                                        DTripPlanner,
                                                                        DStreetMap,
                                                                        DOutTripPlanWriter,
                                                                        DStorageTripPlanWriter);
    CTripPlannerCommandLine CommandLine(Config);
    auto SaveSink = std::make_shared<CStringDataSink>();
    EXPECT_CALL(*DMockFactory,CreateSink("la_480_28_82.html"))
                .WillRepeatedly(testing::Return(SaveSink));
    EXPECT_TRUE(CommandLine.ProcessCommands());
    EXPECT_EQ(DDataOutSink->String(),"> "
                                    " 8:00 AM: Take the F bus from 3rd & K St. (stop 28).\n"
                                    " 8:10 AM: Get off the F bus at 9th & C St. (stop 82).\n"
                                    "> "
                                    "Trip saved to <results>/la_480_28_82.html\n"
                                    "> ");
    
    /*EXPECT_EQ(SaveSink->String(),"mode,node_id\n"
                                 "Walk,10\n"
                                 "Walk,9\n"
                                 "Bus,8\n"
                                 "Bus,7\n"
                                 "Walk,6");*/
    EXPECT_TRUE(DDataErrSink->String().empty());
}

TEST_F(TripPlannerCommandLine, ErrorTest){
    auto InputSource = std::make_shared<CStringDataSource>( "foo\n"
                                                            "stop\n"
                                                            "stop oops\n"
                                                            "toggle\n"
                                                            "toggle  foo\n"
                                                            "set\n"
                                                            "set  label-margin  nope\n"
                                                            "leaveat\n"
                                                            "leaveat   bar   123 900\n"
                                                            "leaveat 8:00AM 82 31\n"
                                                            "arriveby\n"
                                                            "arriveby 123 nope 326\n"
                                                            "arriveby 9:00AM 82 31\n"
                                                            "save\n"
                                                            "exit\n");
    auto Config = std::make_shared<CTripPlannerCommandLine::SConfig>(InputSource,
                                                                        DDataOutSink,
                                                                        DDataErrSink,
                                                                        DMockFactory,
                                                                        DTripPlanner,
                                                                        DStreetMap,
                                                                        DOutTripPlanWriter,
                                                                        DStorageTripPlanWriter);
    CTripPlannerCommandLine CommandLine(Config);

    EXPECT_TRUE(CommandLine.ProcessCommands());
    EXPECT_EQ(DDataOutSink->String(), "> "
                                    "> "
                                    "> "
                                    "> "
                                    "> "
                                    "> "
                                    "> "
                                    "> "
                                    "> "
                                    "> "
                                    "> "
                                    "> "
                                    "> "
                                    "> "
                                    "> ");
    EXPECT_EQ(DDataErrSink->String(),  "Unknown command \"foo\" type help for help.\n"
                                    "Invalid stop command, see help.\n"
                                    "Invalid stop parameter, see help.\n"
                                    "Invalid toggle command, see help.\n"
                                    "Invalid toggle parameter, see help.\n"
                                    "Invalid set command, see help.\n"
                                    "Invalid set parameter, see help.\n"
                                    "Invalid leaveat command, see help.\n"
                                    "Invalid leaveat parameter, see help.\n"
                                    "Unable to find route from 82 to 31.\n"
                                    "Invalid arriveby command, see help.\n"
                                    "Invalid arriveby parameter, see help.\n"
                                    "Unable to find route from 82 to 31.\n"
                                    "No valid trip to save, see help.\n");
}