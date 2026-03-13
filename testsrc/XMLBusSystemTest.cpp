#include <gtest/gtest.h>
#include "XMLBusSystem.h"
#include "StringDataSource.h"
#include "XMLReader.h"

/*
basic test, create a small bus system with 2 stops, creates a path with 1 path from node 321 to node 311
constructs a CXMLBussystem using 2 xmlreader objects
verifies stopcount, routecount, and that stops can be retrieved correctly.
*/
TEST(XMLBusSystemTest, SimpleTest){
    auto BusRouteSource = std::make_shared<CStringDataSource>(  "<bussystem>\n"
                                                                "<stops>\n"
                                                                "   <stop id=\"1\" node=\"321\" description=\"First\"/>\n"
                                                                "   <stop id=\"2\" node=\"311\" description=\"second\"/>\n"
                                                                "</stops>\n"
                                                                "</bussystem>");
    auto BusRouteReader = std::make_shared< CXMLReader >(BusRouteSource);
    auto BusPathSource = std::make_shared<CStringDataSource>(  "<paths>\n"
                                                                "   <path source=\"321\" destination=\"311\"/>\n"
                                                                "      <node id=\"321\"/>\n"
                                                                "      <node id=\"315\"/>\n"
                                                                "      <node id=\"311\"/>\n"
                                                                "   </path>\n"
                                                                "</paths>");
    auto BusPathReader = std::make_shared< CXMLReader >(BusPathSource);
    CXMLBusSystem BusSystem(BusRouteReader,BusPathReader);

    ASSERT_EQ(BusSystem.StopCount(),2);
    EXPECT_EQ(BusSystem.RouteCount(),0);
    EXPECT_NE(BusSystem.StopByIndex(0),nullptr);
    EXPECT_NE(BusSystem.StopByIndex(1),nullptr);
    auto StopObj = BusSystem.StopByID(1);
    ASSERT_NE(StopObj,nullptr);
    EXPECT_EQ(StopObj->ID(),1);
    EXPECT_EQ(StopObj->NodeID(),321);
    EXPECT_EQ(StopObj->Description(),"First");
    StopObj = BusSystem.StopByID(2);
    ASSERT_NE(StopObj,nullptr);
    EXPECT_EQ(StopObj->ID(),2);
    EXPECT_EQ(StopObj->NodeID(),311);
    EXPECT_EQ(StopObj->Description(),"second");

}

//returns nullptr if the index is out of range
/*
behaviors: 
if stopbyindex is called with an index >= stopcount(), it must return nullptr and not crach or wrap around
*/
TEST(XMLBusSystemTest, StopOutBounds){
    //bus system w/ 1 stop
    auto BusRouteSource = std::make_shared<CStringDataSource>(

        "<bussystem>\n"
        "<stops>\n"
        "   <stop id=\"1\" node=\"321\" description=\"First\"/>\n"
        "</stops>\n"
        "</bussystem>"
    );

    auto BusRouteReader = std::make_shared<CXMLReader>(BusRouteSource);

    auto BusPathSource = std::make_shared<CStringDataSource>("<paths></paths>");
    auto BusPathReader = std::make_shared<CXMLReader>(BusPathSource);

    CXMLBusSystem BusSystem(BusRouteReader, BusPathReader);

    //invalid because only 1 stop, out of range if stopcount==1 b/c must return nullptr, very large index should return nullptr(shouldn't have that many stops)
    EXPECT_NE(BusSystem.StopByIndex(0), nullptr);
    EXPECT_EQ(BusSystem.StopByIndex(1), nullptr); //out of range
    EXPECT_EQ(BusSystem.StopByIndex(999), nullptr);//same


}

//stopbyID retunrs nullptr
/*
behavior: if stopbyuid is called with an ID that DNE, return nullptr
*/
TEST(XMLBusSystemTest, StopByIDInvalid){
        //bus system with onyl 1 stop
    auto BusRouteSource = std::make_shared<CStringDataSource>(
        "<bussystem>\n"
        "<stops>\n"
        "   <stop id=\"1\" node=\"321\" description=\"First\"/>\n"
        "</stops>\n"
        "</bussystem>"
    );
    auto BusRouteReader = std::make_shared<CXMLReader>(BusRouteSource);

    auto BusPathSource = std::make_shared<CStringDataSource>("<paths></paths>");
    auto BusPathReader = std::make_shared<CXMLReader>(BusPathSource);

    CXMLBusSystem BusSystem(BusRouteReader, BusPathReader);

    //stop id 999 never defined, returns nullptr
    EXPECT_EQ(BusSystem.StopByID(999), nullptr);
}

//royute parsing and routebyname and invalidstopid
/*
tests the following route behaviors:
    route count must be correct
    routebyname must work
    route stores an ordered list of stop ids
    getstopid with outofrange byst return invalidstopid
    routebyname with an invalid name must retunr nullptr
*/
TEST(XMLBusSystemTest, ParseRoute){
    //bus system with 2 stops, and 1 route named "Jaidon's Route" that visist stop 1 then stop 2
    auto BusRouteSource = std::make_shared<CStringDataSource>(
        "<bussystem>\n"
        "<stops>\n"
        "   <stop id=\"1\" node=\"321\" description=\"First\"/>\n"
        "   <stop id=\"2\" node=\"311\" description=\"second\"/>\n"
        "</stops>\n"
        "<routes>\n"
        "   <route name=\"Blue\">\n"
        "      <routestop stop=\"1\"/>\n"
        "      <routestop stop=\"2\"/>\n"
        "   </route>\n"
        "</routes>\n"
        "</bussystem>"
    );
    auto BusRouteReader = std::make_shared<CXMLReader>(BusRouteSource);

    auto BusPathSource = std::make_shared<CStringDataSource>("<paths></paths>");
    auto BusPathReader = std::make_shared<CXMLReader>(BusPathSource);

    CXMLBusSystem BusSystem(BusRouteReader, BusPathReader);

    //routecoutn must be 1
    ASSERT_EQ(BusSystem.RouteCount(), 1);
    auto RouteObj = BusSystem.RouteByName("Blue");
    ASSERT_NE(RouteObj, nullptr);

    EXPECT_EQ(RouteObj->Name(), "Blue");
    EXPECT_EQ(RouteObj->StopCount(), 2);
    EXPECT_EQ(RouteObj->GetStopID(0), 1);
    EXPECT_EQ(RouteObj->GetStopID(1), 2);
    EXPECT_EQ(RouteObj->GetStopID(2), CBusSystem::InvalidStopID); //out of range
    EXPECT_EQ(BusSystem.RouteByName("Not A Real Route"), nullptr);
}

//pathbystopid works and return invalidnodeid out of range
/*
tests:
    path by stopids with start and end stop ids should return the correct path
    path contains the ordered node traversing
    getnodeid with out of range return invalidnodeid
    if start or end doesnt exist, returns nullptr
*/
TEST(XMLBusSystemTest, PathByStopIDs){
    auto BusRouteSource = std::make_shared<CStringDataSource>(
        "<bussystem>\n"
        "<stops>\n"
        "   <stop id=\"1\" node=\"321\" description=\"First\"/>\n"
        "   <stop id=\"2\" node=\"311\" description=\"second\"/>\n"
        "</stops>\n"
        "</bussystem>"
    );
    auto BusRouteReader = std::make_shared<CXMLReader>(BusRouteSource);

    auto BusPathSource = std::make_shared<CStringDataSource>(
        "<paths>\n"
        "   <path source=\"321\" destination=\"311\">\n"
        "      <node id=\"321\"/>\n"
        "      <node id=\"315\"/>\n"
        "      <node id=\"311\"/>\n"
        "   </path>\n"
        "</paths>"
    );
    auto BusPathReader = std::make_shared<CXMLReader>(BusPathSource);

    CXMLBusSystem BusSystem(BusRouteReader, BusPathReader);

    auto PathObj = BusSystem.PathByStopIDs(1, 2);
    ASSERT_NE(PathObj, nullptr);
//contain 3 node ids
    EXPECT_EQ(PathObj->NodeCount(), 3);
    //traversal order matches the xml from source
    EXPECT_EQ(PathObj->GetNodeID(0), 321);
    EXPECT_EQ(PathObj->GetNodeID(1), 315);
    EXPECT_EQ(PathObj->GetNodeID(2), 311);
    //out of range must return invalid node id
    EXPECT_EQ(PathObj->GetNodeID(3), CStreetMap::InvalidNodeID); //out of range

    // stop DNE, should return nullptr
    EXPECT_EQ(BusSystem.PathByStopIDs(999, 2), nullptr);
    EXPECT_EQ(BusSystem.PathByStopIDs(1, 999), nullptr);
}
