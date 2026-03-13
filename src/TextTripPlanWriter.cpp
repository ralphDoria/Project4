#include "TextTripPlanWriter.h"

#include <chrono>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <any>
#include <unordered_set>

struct CTextTripPlanWriter::SImplementation{

    struct STextConfig final : public CTripPlanWriter::SConfig{
    std::unordered_set<std::string> DEnabledFlags;

    bool FlagEnabled(std::string_view flag) const override{
        return DEnabledFlags.find(std::string(flag)) != DEnabledFlags.end();
    }

    void EnableFlag(std::string_view flag) override{
        if(flag == std::string(CTextTripPlanWriter::Verbose)){
            DEnabledFlags.insert(std::string(flag));
        }
    }

    void DisableFlag(std::string_view flag) override{
        DEnabledFlags.erase(std::string(flag));
    }

    std::any GetOption(std::string_view) const override{
        return std::any{};
    }

    std::unordered_set<std::string> ValidFlags() const override{
        return {std::string(CTextTripPlanWriter::Verbose)};
    }

    EOptionType GetOptionType(std::string_view) const override{
        return EOptionType::None;
    }

    void SetOption(std::string_view, int) override{
    }

    void SetOption(std::string_view, double) override{
    }

    void SetOption(std::string_view, const std::string &) override{
    }

    void ClearOption(std::string_view) override{
    }

    std::unordered_set<std::string> ValidOptions() const override{
        return {};
    }
};


    std::shared_ptr<CBusSystem> DBusSystem;
    std::shared_ptr<SConfig> DConfig;

    SImplementation(std::shared_ptr<CBusSystem> bussystem)
        : DBusSystem(std::move(bussystem)),
          DConfig(std::make_shared<STextConfig>()){
    }

    std::string FormatTime(const CBusSystem::TStopTime &time) const{
        auto TotalMinutes = std::chrono::duration_cast<std::chrono::minutes>(time.to_duration()).count();

        long Hours24 = TotalMinutes / 60;
        long Minutes = TotalMinutes % 60;

        std::string Suffix = (Hours24 >= 12) ? "PM" : "AM";
        long Hours12 = Hours24 % 12;
        if(Hours12 == 0){
            Hours12 = 12;
        }

        std::ostringstream Out;
        Out << std::setw(2) << Hours12
            << ":"
            << std::setfill('0') << std::setw(2) << Minutes
            << " "
            << Suffix;

        auto Result = Out.str();
        if(!Result.empty() && Result[0] == '0'){
            Result[0] = ' ';
        }
        return Result;
    }

    std::shared_ptr<CBusSystem::SStop> StopByID(CBusSystem::TStopID stopid) const{
        return DBusSystem->StopByID(stopid);
    }

    bool WriteLine(std::shared_ptr<CDataSink> sink, const std::string &line) const{
        std::vector<char> Buffer(line.begin(), line.end());
        return sink->Write(Buffer);
    }
};

CTextTripPlanWriter::CTextTripPlanWriter(std::shared_ptr<CBusSystem> bussystem)
    : DImplementation(std::make_unique<SImplementation>(std::move(bussystem))){
}

CTextTripPlanWriter::~CTextTripPlanWriter() = default;

std::shared_ptr<CTripPlanWriter::SConfig> CTextTripPlanWriter::Config() const{
    return DImplementation->DConfig;
}

bool CTextTripPlanWriter::WritePlan(std::shared_ptr<CDataSink> sink, const TTravelPlan &plan){
    if(!sink){
        return false;
    }
    if(plan.size() < 2){
        return false;
    }

    auto VerboseEnabled = DImplementation->DConfig->FlagEnabled(std::string(Verbose));

    for(std::size_t StepIndex = 0; StepIndex + 1 < plan.size(); StepIndex++){
        const auto &Current = plan[StepIndex];
        const auto &Next = plan[StepIndex + 1];

        auto CurrentStop = DImplementation->StopByID(Current.DStopID);
        auto NextStop = DImplementation->StopByID(Next.DStopID);

        if(!CurrentStop || !NextStop){
            return false;
        }

        // Print a "Take the X bus..." line whenever this step begins a bus ride:
        // - first step in the whole plan
        // - or after a transfer (route name changed from previous step)
        if(StepIndex == 0 || plan[StepIndex - 1].DRouteName != Current.DRouteName){
            std::ostringstream TakeOut;
            TakeOut << DImplementation->FormatTime(Current.DTime)
                    << ": Take the " << Current.DRouteName
                    << " bus from " << CurrentStop->Description()
                    << " (stop " << Current.DStopID << ").\n";

            if(!DImplementation->WriteLine(sink, TakeOut.str())){
                return false;
            }
        }

        // Final stop: get off the current bus
        // Final stop: optionally print intermediate "stay on bus" stops, then get off
if(Next.DRouteName.empty()){
    if(VerboseEnabled){
        auto Route = DImplementation->DBusSystem->RouteByName(Current.DRouteName);
        if(!Route){
            return false;
        }

        std::size_t CurrentIndex = Route->StopCount();
        std::size_t NextIndex = Route->StopCount();

        for(std::size_t i = 0; i < Route->StopCount(); i++){
            if(Route->GetStopID(i) == Current.DStopID && CurrentIndex == Route->StopCount()){
                CurrentIndex = i;
            }
            if(Route->GetStopID(i) == Next.DStopID && NextIndex == Route->StopCount()){
                NextIndex = i;
            }
        }

        if(CurrentIndex == Route->StopCount() || NextIndex == Route->StopCount() || CurrentIndex > NextIndex){
            return false;
        }

        std::size_t MatchingTrip = Route->TripCount();
        for(std::size_t trip = 0; trip < Route->TripCount(); trip++){
            if(Route->GetStopTime(CurrentIndex, trip) == Current.DTime &&
               Route->GetStopTime(NextIndex, trip) == Next.DTime){
                MatchingTrip = trip;
                break;
            }
        }

        if(MatchingTrip == Route->TripCount()){
            return false;
        }

        for(std::size_t stop = CurrentIndex + 1; stop < NextIndex; stop++){
            auto MidStopID = Route->GetStopID(stop);
            auto MidStop = DImplementation->StopByID(MidStopID);
            if(!MidStop){
                return false;
            }

            std::ostringstream StayOut;
            StayOut << DImplementation->FormatTime(Route->GetStopTime(stop, MatchingTrip))
                    << ": Stay on the " << Current.DRouteName
                    << " bus at " << MidStop->Description()
                    << " (stop " << MidStopID << ").\n";

            if(!DImplementation->WriteLine(sink, StayOut.str())){
                return false;
            }
        }
    }

    std::ostringstream OffOut;
    OffOut << DImplementation->FormatTime(Next.DTime)
           << ": Get off the " << Current.DRouteName
           << " bus at " << NextStop->Description()
           << " (stop " << Next.DStopID << ").\n";

    if(!DImplementation->WriteLine(sink, OffOut.str())){
        return false;
    }
}
        // Transfer to another bus
        else if(Current.DRouteName != Next.DRouteName){
            std::ostringstream TransferOut;
            TransferOut << "        : Get off the " << Current.DRouteName
                        << " bus at " << NextStop->Description()
                        << " (stop " << Next.DStopID << ") and wait for the "
                        << Next.DRouteName << " bus.\n";

            if(!DImplementation->WriteLine(sink, TransferOut.str())){
                return false;
            }
        }
        // Staying on the same bus at an intermediate stop (verbose mode only)
        else if(VerboseEnabled){
            std::ostringstream StayOut;
            StayOut << DImplementation->FormatTime(Next.DTime)
                    << ": Stay on the " << Current.DRouteName
                    << " bus at " << NextStop->Description()
                    << " (stop " << Next.DStopID << ").\n";

            if(!DImplementation->WriteLine(sink, StayOut.str())){
                return false;
            }
        }
    }

    return true;
}