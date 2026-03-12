#ifndef MOCKSTREETMAP_H
#define MOCKSTREETMAP_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "StreetMap.h"


struct SMockNode : public CStreetMap::SNode{
    public:
        MOCK_METHOD(CStreetMap::TNodeID, ID, (), (const, noexcept, override));
        MOCK_METHOD(CStreetMap::SLocation, Location, (), (const, noexcept, override));
        MOCK_METHOD(std::size_t, AttributeCount, (), (const, noexcept, override));
        MOCK_METHOD(std::string, GetAttributeKey, (std::size_t index), (noexcept, const, override));
        MOCK_METHOD(bool, HasAttribute, (const std::string &key), (noexcept, const, override));
        MOCK_METHOD(std::string, GetAttribute, (const std::string &key), (noexcept, const, override));
};

struct SMockWay : public CStreetMap::SWay{
    public:
        MOCK_METHOD(CStreetMap::TWayID, ID, (), (const, noexcept, override));
        MOCK_METHOD(std::size_t, NodeCount, (), (const, noexcept, override));
        MOCK_METHOD(CStreetMap::TNodeID, GetNodeID, (std::size_t index), (const, noexcept, override));
        MOCK_METHOD(std::size_t, AttributeCount, (), (const, noexcept, override));
        MOCK_METHOD(std::string, GetAttributeKey, (std::size_t index), (noexcept, const, override));
        MOCK_METHOD(bool, HasAttribute, (const std::string &key), (noexcept, const, override));
        MOCK_METHOD(std::string, GetAttribute, (const std::string &key), (noexcept, const, override));
};

class CMockStreetMap : public CStreetMap{
    public:
        MOCK_METHOD(std::size_t, NodeCount, (), (const, noexcept, override));
        MOCK_METHOD(std::size_t, WayCount, (), (const, noexcept, override));

        MOCK_METHOD(std::shared_ptr<SNode>, NodeByIndex, (std::size_t), (const, noexcept, override));

        MOCK_METHOD(std::shared_ptr<SNode>, NodeByID, (TNodeID), (const, noexcept, override));

        MOCK_METHOD(std::shared_ptr<SWay>, WayByIndex, (std::size_t), (const, noexcept, override));

        MOCK_METHOD(std::shared_ptr<SWay>, WayByID, (TWayID), (const, noexcept, override));

        static std::shared_ptr<CMockStreetMap> CreateTestStreetMap();
};

#endif