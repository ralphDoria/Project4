#ifndef TRIPWRITER_H
#define TRIPWRITER_H

#include "TripPlanner.h"
#include "DataSink.h"
#include <any>
#include <memory>
#include <string_view>
#include <unordered_set>

class CTripPlanWriter{
    public:
        using TTravelPlan = CTripPlanner::TTravelPlan;

        struct SConfig{
            enum class EOptionType{None, Int, Double, String};
            virtual bool FlagEnabled(std::string_view flag) const = 0;
            virtual void EnableFlag(std::string_view flag) = 0;
            virtual void DisableFlag(std::string_view flag) = 0;
            virtual std::any GetOption(std::string_view option) const = 0;
            virtual std::unordered_set<std::string> ValidFlags() const = 0;
            virtual EOptionType GetOptionType(std::string_view option) const = 0;
            virtual void SetOption(std::string_view option, int value) = 0;
            virtual void SetOption(std::string_view option, double value) = 0;
            virtual void SetOption(std::string_view option, const std::string &value) = 0;
            virtual void ClearOption(std::string_view option) = 0;
            virtual std::unordered_set<std::string> ValidOptions() const = 0;
        };
        virtual ~CTripPlanWriter(){};

        virtual std::shared_ptr<SConfig> Config() const = 0;

        virtual bool WritePlan(std::shared_ptr<CDataSink> sink, const TTravelPlan &plan) = 0;
};

#endif
