#include <gtest/gtest.h>
#include "OpenStreetMap.h"
#include "StringDataSource.h"

TEST(OpenStreetMapTest, SimpleTest){
    auto OSMSource = std::make_shared<CStringDataSource>(  "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
	                                                            "  <node id=\"1\" lat=\"38.5\" lon=\"-121.7\"/>\n"
	                                                            "  <node id=\"2\" lat=\"38.5\" lon=\"-121.8\"/>\n"
                                                                "  <way id=\"1000\">\n"
                                                                "    <nd ref=\"1\"/>\n"
		                                                        "    <nd ref=\"2\"/>\n"
                                                                "  </way>\n"
                                                                "</osm>"
                                                            );
    auto OSMReader = std::make_shared< CXMLReader >(OSMSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    ASSERT_EQ(OpenStreetMap.NodeCount(), 2);
    auto Node = OpenStreetMap.NodeByIndex(0);
    ASSERT_NE(Node,nullptr);
    EXPECT_EQ(Node->ID(),1);
    auto Location = CStreetMap::SLocation{38.5,-121.7};
    EXPECT_EQ(Node->Location(),Location);
    ASSERT_EQ(OpenStreetMap.WayCount(), 1);

}

TEST(OpenStreetMapTest, NodeByIDAndOutOfBounds){
    auto OSMSource = std::make_shared<CStringDataSource>(
        "<osm version=\"0.6\">\n"
        "  <node id=\"10\" lat=\"1.0\" lon=\"2.0\"/>\n"
        "</osm>\n"
    );

    auto OSMReader = std::make_shared<CXMLReader>(OSMSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    ASSERT_EQ(OpenStreetMap.NodeCount(), 1);

    //valid
    auto Node = OpenStreetMap.NodeByID(10);
    ASSERT_NE(Node, nullptr);
    EXPECT_EQ(Node->ID(), 10);
    EXPECT_EQ(Node->Location(), (CStreetMap::SLocation{1.0, 2.0}));

    //invalidid returns nullptr
    EXPECT_EQ(OpenStreetMap.NodeByID(999), nullptr);

    // outofbounds index return nullptr
    EXPECT_EQ(OpenStreetMap.NodeByIndex(1), nullptr);
}

TEST(OpenStreetMapTest, WayByIDByIndexAndNodeRefs){
    auto OSMSource = std::make_shared<CStringDataSource>(
        "<osm version=\"0.6\">\n"
        "  <node id=\"1\" lat=\"0\" lon=\"0\"/>\n"
        "  <node id=\"2\" lat=\"0\" lon=\"0\"/>\n"
        "  <node id=\"3\" lat=\"0\" lon=\"0\"/>\n"
        "  <way id=\"42\">\n"
        "    <nd ref=\"1\"/>\n"
        "    <nd ref=\"2\"/>\n"
        "    <nd ref=\"3\"/>\n"
        "  </way>\n"
        "</osm>\n"
    );

    auto OSMReader = std::make_shared<CXMLReader>(OSMSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    ASSERT_EQ(OpenStreetMap.WayCount(), 1);

    //waybyindex
    auto Way0 = OpenStreetMap.WayByIndex(0);
    ASSERT_NE(Way0, nullptr);
    EXPECT_EQ(Way0->ID(), 42);

    //waybyid
    auto Way = OpenStreetMap.WayByID(42);
    ASSERT_NE(Way, nullptr);
    EXPECT_EQ(Way->ID(), 42);

    //way node ref
    EXPECT_EQ(Way->NodeCount(), 3);
    EXPECT_EQ(Way->GetNodeID(0), 1);
    EXPECT_EQ(Way->GetNodeID(1), 2);
    EXPECT_EQ(Way->GetNodeID(2), 3);

    //outofrange node index should return invalidnodeid
    EXPECT_EQ(Way->GetNodeID(99), CStreetMap::InvalidNodeID);
}

TEST(OpenStreetMapTest, WayInvalidIDAndOutOfBounds){
    auto OSMSource = std::make_shared<CStringDataSource>(
        "<osm version=\"0.6\">\n"
        "  <way id=\"7\"></way>\n"
        "</osm>\n"
    );

    auto OSMReader = std::make_shared<CXMLReader>(OSMSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    ASSERT_EQ(OpenStreetMap.WayCount(), 1);

    EXPECT_NE(OpenStreetMap.WayByIndex(0), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(1), nullptr);

    EXPECT_NE(OpenStreetMap.WayByID(7), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByID(999), nullptr);
}


TEST(OpenStreetMapTest, NodeOutOfBoundsAndInvalidID){
    auto OSMSource = std::make_shared<CStringDataSource>(
        "<osm version=\"0.6\">"
        "  <node id=\"1\" lat=\"38.5\" lon=\"-121.7\"/>"
        "</osm>"
    );
    auto OSMReader = std::make_shared<CXMLReader>(OSMSource);
    COpenStreetMap Map(OSMReader);

    EXPECT_EQ(Map.NodeByIndex(999), nullptr);
    EXPECT_EQ(Map.NodeByID(999), nullptr);
}

TEST(OpenStreetMapTest, WayOutOfBoundsAndInvalidID){
    auto OSMSource = std::make_shared<CStringDataSource>(
        "<osm version=\"0.6\">"
        "  <node id=\"1\" lat=\"38.5\" lon=\"-121.7\"/>"
        "  <node id=\"2\" lat=\"38.5\" lon=\"-121.8\"/>"
        "  <way id=\"1000\">"
        "    <nd ref=\"1\"/>"
        "    <nd ref=\"2\"/>"
        "  </way>"
        "</osm>"
    );
    auto OSMReader = std::make_shared<CXMLReader>(OSMSource);
    COpenStreetMap Map(OSMReader);

    EXPECT_EQ(Map.WayByIndex(999), nullptr);
    EXPECT_EQ(Map.WayByID(999999), nullptr);
}

TEST(OpenStreetMapTest, NodeAndWayAttributesAndBounds){
    auto OSMSource = std::make_shared<CStringDataSource>(
        "<osm version=\"0.6\">"
        "  <node id=\"1\" lat=\"38.5\" lon=\"-121.7\">"
        "    <tag k=\"name\" v=\"StopOne\"/>"
        "  </node>"
        "  <node id=\"2\" lat=\"38.5\" lon=\"-121.8\"/>"
        "  <way id=\"1000\">"
        "    <nd ref=\"1\"/>"
        "    <nd ref=\"2\"/>"
        "    <tag k=\"highway\" v=\"residential\"/>"
        "  </way>"
        "</osm>"
    );
    auto OSMReader = std::make_shared<CXMLReader>(OSMSource);
    COpenStreetMap Map(OSMReader);

    auto N = Map.NodeByID(1);
    ASSERT_NE(N, nullptr);
    EXPECT_EQ(N->AttributeCount(), 1);
    EXPECT_TRUE(N->HasAttribute("name"));
    EXPECT_EQ(N->GetAttribute("name"), "StopOne");
    EXPECT_EQ(N->GetAttribute("missing"), "");
    EXPECT_EQ(N->GetAttributeKey(0), "name");
    EXPECT_EQ(N->GetAttributeKey(99), "");

    auto W = Map.WayByID(1000);
    ASSERT_NE(W, nullptr);
    EXPECT_EQ(W->NodeCount(), 2);
    EXPECT_EQ(W->GetNodeID(0), 1);
    EXPECT_EQ(W->GetNodeID(1), 2);
    EXPECT_EQ(W->GetNodeID(99), CStreetMap::InvalidNodeID);

    EXPECT_EQ(W->AttributeCount(), 1);
    EXPECT_TRUE(W->HasAttribute("highway"));
    EXPECT_EQ(W->GetAttribute("highway"), "residential");
    EXPECT_EQ(W->GetAttribute("missing"), "");
    EXPECT_EQ(W->GetAttributeKey(0), "highway");
    EXPECT_EQ(W->GetAttributeKey(99), "");
}

TEST(OpenStreetMapTest, ParseEdgeCases){
    // Missing <osm> tag should produce empty map
    auto badSrc = std::make_shared<CStringDataSource>("<notosm/>");
    auto badReader = std::make_shared<CXMLReader>(badSrc);
    COpenStreetMap badMap(badReader);
    EXPECT_EQ(badMap.NodeCount(), 0);
    EXPECT_EQ(badMap.WayCount(), 0);

    // Empty tag key + empty nd ref
    auto src = std::make_shared<CStringDataSource>(
        "<osm version=\"0.6\">"
        "  <node id=\"1\" lat=\"0\" lon=\"0\">"
        "    <tag k=\"\" v=\"x\"/>"
        "  </node>"
        "  <way id=\"10\">"
        "    <nd ref=\"\"/>"
        "    <tag k=\"highway\" v=\"residential\"/>"
        "  </way>"
        "</osm>"
    );
    auto reader = std::make_shared<CXMLReader>(src);
    COpenStreetMap map(reader);

    EXPECT_EQ(map.NodeCount(), 1);
    EXPECT_EQ(map.WayCount(), 1);
}