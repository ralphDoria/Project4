#ifndef MOCKBUSSYSTEM_H
#define MOCKBUSSYSTEM_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "BusSystem.h"


struct SMockStop : public CBusSystem::SStop{
    public:
        MOCK_METHOD(CBusSystem::TStopID, ID, (), (const, noexcept, override));
        MOCK_METHOD(CStreetMap::TNodeID, NodeID, (), (const, noexcept, override));
        MOCK_METHOD(std::string, Description, (), (const, noexcept, override));
        MOCK_METHOD(std::string, Description, (const std::string &description), (noexcept, override));
};

struct SMockRoute : public CBusSystem::SRoute{
    public:
        MOCK_METHOD(std::string, Name, (), (const, noexcept, override));
        MOCK_METHOD(std::size_t, StopCount, (), (const, noexcept, override));
        MOCK_METHOD(std::size_t, TripCount, (), (const, noexcept, override));
        MOCK_METHOD(CBusSystem::TStopID, GetStopID, (std::size_t), (const, noexcept, override));
        MOCK_METHOD(CBusSystem::TStopTime, GetStopTime, (std::size_t, std::size_t), (const, noexcept, override));
};

struct SMockPath : public CBusSystem::SPath{
    public:
        MOCK_METHOD(CStreetMap::TNodeID, StartNodeID, (), (const, noexcept, override));
        MOCK_METHOD(CStreetMap::TNodeID, EndNodeID, (), (const, noexcept, override));
        MOCK_METHOD(std::size_t, NodeCount, (), (const, noexcept, override));
        MOCK_METHOD(CStreetMap::TNodeID, GetNodeID, (std::size_t), (const, noexcept, override));
};

class CMockBusSystem : public CBusSystem{
    public:
        MOCK_METHOD(std::size_t, StopCount, (), (const, noexcept, override));
        MOCK_METHOD(std::size_t, RouteCount, (), (const, noexcept, override));

        MOCK_METHOD(std::shared_ptr<SStop>, StopByIndex, (std::size_t), (const, noexcept, override));

        MOCK_METHOD(std::shared_ptr<SStop>, StopByID, (TStopID), (const, noexcept, override));

        MOCK_METHOD(std::shared_ptr<SRoute>, RouteByIndex, (std::size_t), (const, noexcept, override));

        MOCK_METHOD(std::shared_ptr<SRoute>, RouteByName, (const std::string &), (const, noexcept, override));

        MOCK_METHOD(std::shared_ptr<SPath>, PathByStopIDs, (TStopID, TStopID), (const, noexcept, override));

        static std::shared_ptr<CMockBusSystem> CreateTestBusSystem();
};

#endif