#include <gtest/gtest.h>
#include "StreetMap.h"

//minimal node to call base default
struct TNode : public CStreetMap::SNode{
    CStreetMap::TNodeID ID() const noexcept override { return 1; }
    CStreetMap::SLocation Location() const noexcept override { return CStreetMap::SLocation{0.0, 0.0}; }
    std::size_t AttributeCount() const noexcept override { return 0; }
    std::string GetAttributeKey(std::size_t) const noexcept override { return ""; }
    bool HasAttribute(const std::string &) const noexcept override { return false; }
    std::string GetAttribute(const std::string &) const noexcept override { return ""; }

    void SetAttribute(const std::string &k, const std::string &v) override{
        CStreetMap::SNode::SetAttribute(k, v);
    }
};

struct TWay : public CStreetMap::SWay{
    CStreetMap::TWayID ID() const noexcept override { return 1; }
    std::size_t NodeCount() const noexcept override { return 0; }
    CStreetMap::TNodeID GetNodeID(std::size_t) const noexcept override { return CStreetMap::InvalidNodeID; }
    std::size_t AttributeCount() const noexcept override { return 0; }
    std::string GetAttributeKey(std::size_t) const noexcept override { return ""; }
    bool HasAttribute(const std::string &) const noexcept override { return false; }
    std::string GetAttribute(const std::string &) const noexcept override { return ""; }

    void AddNodeID(CStreetMap::TNodeID nodeid) override{
        CStreetMap::SWay::AddNodeID(nodeid);
    }

    void SetAttribute(const std::string &k, const std::string &v) override{
        CStreetMap::SWay::SetAttribute(k, v);
    }
};


TEST(StreetMapTest, DefaultImplsAreCallable){
    TNode node;
    node.SetAttribute("k","v");

    TWay way;
    way.AddNodeID(123); 
    way.SetAttribute("a","b");

    SUCCEED();
}
