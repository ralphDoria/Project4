#include "TripPlannerCommandLine.h"

#include <any>
#include <charconv>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

struct CTripPlannerCommandLine::SImplementation{
    std::shared_ptr<SConfig> DConfig;
    CTripPlanner::TTravelPlan DLastPlan;
    std::string DLastSaveName;
    bool DHaveValidTrip = false;

    explicit SImplementation(std::shared_ptr<SConfig> config)
        : DConfig(std::move(config)){
    }

    static bool WriteString(const std::shared_ptr<CDataSink> &sink, const std::string &str){
        if(!sink){
            return false;
        }
        std::vector<char> Buffer(str.begin(), str.end());
        return sink->Write(Buffer);
    }

    bool WriteOut(const std::string &str) const{
        return WriteString(DConfig->DOutSink, str);
    }

    bool WriteErr(const std::string &str) const{
        return WriteString(DConfig->DErrorSink, str);
    }

    static std::string Trim(const std::string &str){
        std::size_t Start = 0;
        while(Start < str.size() && std::isspace(static_cast<unsigned char>(str[Start]))){
            Start++;
        }
        std::size_t End = str.size();
        while(End > Start && std::isspace(static_cast<unsigned char>(str[End - 1]))){
            End--;
        }
        return str.substr(Start, End - Start);
    }

    static std::vector<std::string> Tokenize(const std::string &str){
        std::vector<std::string> Tokens;
        std::istringstream Input(str);
        std::string Token;
        while(Input >> Token){
            Tokens.push_back(Token);
        }
        return Tokens;
    }

    static bool ParseSizeT(const std::string &str, std::size_t &value){
        value = 0;
        auto Begin = str.data();
        auto End = str.data() + str.size();
        auto Result = std::from_chars(Begin, End, value);
        return (Result.ec == std::errc{} && Result.ptr == End);
    }

    static bool ParseStopTime(const std::string &str, CBusSystem::TStopTime &time){
        std::string Upper;
        Upper.reserve(str.size());
        for(char Ch : str){
            Upper.push_back(std::toupper(static_cast<unsigned char>(Ch)));
        }

        bool HasAM = false;
        bool HasPM = false;

        if(Upper.size() >= 2){
            auto Suffix = Upper.substr(Upper.size() - 2);
            if(Suffix == "AM"){
                HasAM = true;
                Upper = Upper.substr(0, Upper.size() - 2);
            }
            else if(Suffix == "PM"){
                HasPM = true;
                Upper = Upper.substr(0, Upper.size() - 2);
            }
        }

        auto Colon = Upper.find(':');
        if(Colon == std::string::npos){
            return false;
        }

        std::string HourString = Upper.substr(0, Colon);
        std::string MinuteString = Upper.substr(Colon + 1);

        int Hour = 0;
        int Minute = 0;

        if(!ParseInt(HourString, Hour) || !ParseInt(MinuteString, Minute)){
            return false;
        }
        if(Minute < 0 || Minute >= 60){
            return false;
        }

        if(HasAM || HasPM){
            if(Hour < 1 || Hour > 12){
                return false;
            }
            if(Hour == 12){
                Hour = 0;
            }
            if(HasPM){
                Hour += 12;
            }
        }
        else{
            if(Hour < 0 || Hour >= 24){
                return false;
            }
        }

        time = CBusSystem::TStopTime(std::chrono::hours(Hour) + std::chrono::minutes(Minute));
        return true;
    }

    bool ReadCommandLine(std::string &line) const{
        line.clear();

        std::vector<char> Buffer;
        while(true){
            Buffer.clear();
            if(!DConfig->DCommandSource->Read(Buffer, 1)){
                return !line.empty();
            }
            if(Buffer.empty()){
                return !line.empty();
            }

            char Ch = Buffer[0];
            if(Ch == '\r'){
                continue;
            }
            if(Ch == '\n'){
                return true;
            }
            line.push_back(Ch);
        }
    }

    static bool ParseInt(const std::string &str, int &value){
        value = 0;
        auto Begin = str.data();
        auto End = str.data() + str.size();
        auto Result = std::from_chars(Begin, End, value);
        return (Result.ec == std::errc{} && Result.ptr == End);
    }

    static bool ParseDouble(const std::string &str, double &value){
        std::istringstream Input(str);
        Input >> value;
        return !Input.fail() && Input.eof();
    }

    static std::string FormatBool(bool value){
        return value ? "true" : "false";
    }

    static std::string FormatInt(int value){
        return std::to_string(value);
    }

    static std::string FormatDouble(double value){
        std::ostringstream Out;
        Out << std::fixed << std::setprecision(6) << value;
        return Out.str();
    }

    static std::string FormatLocation(const CStreetMap::SLocation &loc){
        auto FormatCoordinate = [](double value, char positive, char negative){
            char Dir = value >= 0.0 ? positive : negative;
            double AbsValue = std::abs(value);

            int Degrees = static_cast<int>(AbsValue);
            double MinutesFull = (AbsValue - Degrees) * 60.0;
            int Minutes = static_cast<int>(MinutesFull);
            double Seconds = (MinutesFull - Minutes) * 60.0;

            std::ostringstream Out;
            Out << Degrees << "d "
                << Minutes << "' "
                << std::fixed << std::setprecision(2) << Seconds << "\" "
                << Dir;
            return Out.str();
        };

        return FormatCoordinate(loc.DLatitude, 'N', 'S') + ", " +
               FormatCoordinate(loc.DLongitude, 'E', 'W');
    }

    static std::string SaveFileNameLeaveAt(const CBusSystem::TStopTime &time, CBusSystem::TStopID src, CBusSystem::TStopID dst){
        auto Minutes = std::chrono::duration_cast<std::chrono::minutes>(time.to_duration()).count();
        std::ostringstream Out;
        Out << "la_" << Minutes << "_" << src << "_" << dst << ".html";
        return Out.str();
    }

    static std::string SaveFileNameArriveBy(const CBusSystem::TStopTime &time, CBusSystem::TStopID src, CBusSystem::TStopID dst){
        auto Minutes = std::chrono::duration_cast<std::chrono::minutes>(time.to_duration()).count();
        std::ostringstream Out;
        Out << "ab_" << Minutes << "_" << src << "_" << dst << ".html";
        return Out.str();
    }

    bool PrintPrompt() const{
        return WriteOut("> ");
    }

    bool PrintHelp() const{
        return WriteOut("--------------------------------------------------------------------------\n"
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
                        "save     Saves the last calculated trip to file\n");
    }

    bool PrintCount() const{
        std::ostringstream Out;
        Out << DConfig->DTripPlanner->BusSystemIndexer()->StopCount() << " stops\n";
        return WriteOut(Out.str());
    }

    bool PrintConfig() const{
        auto OutConfig = DConfig->DOutWriter->Config();
        auto StorageConfig = DConfig->DStorageWriter->Config();

        std::vector<std::string> FlagOrder = {
            "motorway-enabled",
            "primary-enabled",
            "residential-enabled",
            "secondary-enabled",
            "tertiary-enabled",
            "verbose"
        };

        std::vector<std::string> OptionOrder = {
            "bus-color-0",
            "bus-color-1",
            "bus-stroke",
            "busstop-radius",
            "destination-color",
            "destination-radius",
            "label-background",
            "label-color",
            "label-margin",
            "label-paint-order",
            "label-size",
            "motorway-stroke",
            "primary-stroke",
            "residential-stroke",
            "secondary-stroke",
            "source-color",
            "source-radius",
            "street-color",
            "svg-height",
            "svg-margin",
            "svg-width",
            "tertiary-stroke"
        };

        std::size_t MaxKeyLength = 0;
        for(const auto &Key : FlagOrder){
            MaxKeyLength = std::max(MaxKeyLength, Key.size());
        }
        for(const auto &Key : OptionOrder){
            MaxKeyLength = std::max(MaxKeyLength, Key.size());
        }

        std::ostringstream Out;

        for(const auto &Key : FlagOrder){
            bool Value = false;
            if(OutConfig && OutConfig->ValidFlags().find(Key) != OutConfig->ValidFlags().end()){
                Value = OutConfig->FlagEnabled(Key);
            }
            else if(StorageConfig && StorageConfig->ValidFlags().find(Key) != StorageConfig->ValidFlags().end()){
                Value = StorageConfig->FlagEnabled(Key);
            }

            Out << std::left << std::setw(static_cast<int>(MaxKeyLength)) << Key
                << ": " << FormatBool(Value) << "\n";
        }

        for(const auto &Key : OptionOrder){
            auto Type = StorageConfig->GetOptionType(Key);
            auto Value = StorageConfig->GetOption(Key);

            Out << std::left << std::setw(static_cast<int>(MaxKeyLength)) << Key << ": ";

            if(Type == CTripPlanWriter::SConfig::EOptionType::Int){
                Out << FormatInt(std::any_cast<int>(Value));
            }
            else if(Type == CTripPlanWriter::SConfig::EOptionType::Double){
                Out << FormatDouble(std::any_cast<double>(Value));
            }
            else if(Type == CTripPlanWriter::SConfig::EOptionType::String){
                Out << std::any_cast<std::string>(Value);
            }
            Out << "\n";
        }

        return WriteOut(Out.str());
    }

    bool DoToggle(const std::vector<std::string> &Tokens){
        if(Tokens.size() != 2){
            return WriteErr("Invalid toggle command, see help.\n");
        }

        const auto &Flag = Tokens[1];
        auto OutConfig = DConfig->DOutWriter->Config();
        auto StorageConfig = DConfig->DStorageWriter->Config();

        bool Valid = false;
        if(OutConfig && OutConfig->ValidFlags().find(Flag) != OutConfig->ValidFlags().end()){
            Valid = true;
            if(OutConfig->FlagEnabled(Flag)){
                OutConfig->DisableFlag(Flag);
            }
            else{
                OutConfig->EnableFlag(Flag);
            }
        }
        if(StorageConfig && StorageConfig->ValidFlags().find(Flag) != StorageConfig->ValidFlags().end()){
            Valid = true;
            if(StorageConfig->FlagEnabled(Flag)){
                StorageConfig->DisableFlag(Flag);
            }
            else{
                StorageConfig->EnableFlag(Flag);
            }
        }

        if(!Valid){
            return WriteErr("Invalid toggle parameter, see help.\n");
        }

        bool NewValue = false;
        if(OutConfig && OutConfig->ValidFlags().find(Flag) != OutConfig->ValidFlags().end()){
            NewValue = OutConfig->FlagEnabled(Flag);
        }
        else if(StorageConfig && StorageConfig->ValidFlags().find(Flag) != StorageConfig->ValidFlags().end()){
            NewValue = StorageConfig->FlagEnabled(Flag);
        }

        std::ostringstream Out;
        Out << "Flag " << Flag << " is now " << FormatBool(NewValue) << "\n";
        return WriteOut(Out.str());
    }

    bool DoSet(const std::vector<std::string> &Tokens){
        if(Tokens.size() != 3){
            return WriteErr("Invalid set command, see help.\n");
        }

        const auto &Option = Tokens[1];
        const auto &ValueText = Tokens[2];
        auto StorageConfig = DConfig->DStorageWriter->Config();

        auto Type = StorageConfig->GetOptionType(Option);
        if(Type == CTripPlanWriter::SConfig::EOptionType::None){
            return WriteErr("Invalid set parameter, see help.\n");
        }

        std::ostringstream Out;
        Out << "Option " << Option << " is now ";

        if(Type == CTripPlanWriter::SConfig::EOptionType::Int){
            int Value;
            if(!ParseInt(ValueText, Value)){
                return WriteErr("Invalid set parameter, see help.\n");
            }
            StorageConfig->SetOption(Option, Value);
            Out << Value;
        }
        else if(Type == CTripPlanWriter::SConfig::EOptionType::Double){
            double Value;
            if(!ParseDouble(ValueText, Value)){
                return WriteErr("Invalid set parameter, see help.\n");
            }
            StorageConfig->SetOption(Option, Value);
            Out << FormatDouble(Value);
        }
        else if(Type == CTripPlanWriter::SConfig::EOptionType::String){
            StorageConfig->SetOption(Option, ValueText);
            Out << ValueText;
        }
        else{
            return WriteErr("Invalid set parameter, see help.\n");
        }

        Out << "\n";
        return WriteOut(Out.str());
    }

    bool DoStop(const std::vector<std::string> &Tokens) const{
        if(Tokens.size() != 2){
            return WriteErr("Invalid stop command, see help.\n");
        }

        std::size_t Index;
        if(!ParseSizeT(Tokens[1], Index)){
            return WriteErr("Invalid stop parameter, see help.\n");
        }

        auto Stop = DConfig->DTripPlanner->BusSystemIndexer()->SortedStopByIndex(Index);
        if(!Stop){
            return WriteErr("Invalid stop parameter, see help.\n");
        }

        auto Node = DConfig->DStreetMap->NodeByID(Stop->NodeID());
        if(!Node){
            return WriteErr("Invalid stop parameter, see help.\n");
        }

        std::ostringstream Out;
        Out << "Stop " << Index << ":\n"
            << "    ID          : " << Stop->ID() << "\n"
            << "    Node ID     : " << Stop->NodeID() << "\n"
            << "    Location    : " << FormatLocation(Node->Location()) << "\n"
            << "    Description : " << Stop->Description() << "\n";

        return WriteOut(Out.str());
    }

    bool DoLeaveAt(const std::vector<std::string> &Tokens){
        if(Tokens.size() != 4){
            return WriteErr("Invalid leaveat command, see help.\n");
        }

        std::size_t Src, Dst;
        if(!ParseSizeT(Tokens[2], Src) || !ParseSizeT(Tokens[3], Dst)){
            return WriteErr("Invalid leaveat parameter, see help.\n");
        }

        CBusSystem::TStopTime Time;
        if(!ParseStopTime(Tokens[1], Time)){
            return WriteErr("Invalid leaveat parameter, see help.\n");
        }   

        CTripPlanner::TTravelPlan Plan;
        if(!DConfig->DTripPlanner->FindRouteLeaveTime(Src, Dst, Time, Plan)){
            std::ostringstream Err;
            Err << "Unable to find route from " << Src << " to " << Dst << ".\n";
            DHaveValidTrip = false;
            DLastPlan.clear();
            return WriteErr(Err.str());
        }

        DLastPlan = Plan;
        DLastSaveName = SaveFileNameLeaveAt(Time, Src, Dst);
        DHaveValidTrip = true;
        return DConfig->DOutWriter->WritePlan(DConfig->DOutSink, DLastPlan);
    }

    bool DoArriveBy(const std::vector<std::string> &Tokens){
        if(Tokens.size() != 4){
            return WriteErr("Invalid arriveby command, see help.\n");
        }

        std::size_t Src, Dst;
        if(!ParseSizeT(Tokens[2], Src) || !ParseSizeT(Tokens[3], Dst)){
            return WriteErr("Invalid arriveby parameter, see help.\n");
        }

        CBusSystem::TStopTime Time;
        if(!ParseStopTime(Tokens[1], Time)){
            return WriteErr("Invalid arriveby parameter, see help.\n");
        }

        CTripPlanner::TTravelPlan Plan;
        if(!DConfig->DTripPlanner->FindRouteArrivalTime(Src, Dst, Time, Plan)){
            std::ostringstream Err;
            Err << "Unable to find route from " << Src << " to " << Dst << ".\n";
            DHaveValidTrip = false;
            DLastPlan.clear();
            return WriteErr(Err.str());
        }

        DLastPlan = Plan;
        DLastSaveName = SaveFileNameArriveBy(Time, Src, Dst);
        DHaveValidTrip = true;
        return DConfig->DOutWriter->WritePlan(DConfig->DOutSink, DLastPlan);
    }

    bool DoSave(){
        if(!DHaveValidTrip || DLastPlan.empty()){
            return WriteErr("No valid trip to save, see help.\n");
        }

        auto Sink = DConfig->DResultsFactory->CreateSink(DLastSaveName);
        if(!Sink){
            return false;
        }

        if(!DConfig->DStorageWriter->WritePlan(Sink, DLastPlan)){
            return false;
        }

        std::ostringstream Out;
        Out << "Trip saved to <results>/" << DLastSaveName << "\n";
        return WriteOut(Out.str());
    }
};

CTripPlannerCommandLine::CTripPlannerCommandLine(std::shared_ptr<SConfig> config)
    : DImplementation(std::make_unique<SImplementation>(std::move(config))){
}

CTripPlannerCommandLine::~CTripPlannerCommandLine() = default;

bool CTripPlannerCommandLine::ProcessCommands(){
    std::string CommandLine;

    while(true){
        if(!DImplementation->PrintPrompt()){
            return false;
        }

        if(!DImplementation->ReadCommandLine(CommandLine)){
            return true;
        }

        auto Trimmed = SImplementation::Trim(CommandLine);
        auto Tokens = SImplementation::Tokenize(Trimmed);

        if(Tokens.empty()){
            continue;
        }

        const auto &Command = Tokens[0];

        if(Command == "exit"){
            return true;
        }
        else if(Command == "help"){
            if(!DImplementation->PrintHelp()){
                return false;
            }
        }
        else if(Command == "count"){
            if(!DImplementation->PrintCount()){
                return false;
            }
        }
        else if(Command == "config"){
            if(!DImplementation->PrintConfig()){
                return false;
            }
        }
        else if(Command == "toggle"){
            if(!DImplementation->DoToggle(Tokens)){
                return false;
            }
        }
        else if(Command == "set"){
            if(!DImplementation->DoSet(Tokens)){
                return false;
            }
        }
        else if(Command == "stop"){
            if(!DImplementation->DoStop(Tokens)){
                return false;
            }
        }
        else if(Command == "leaveat"){
            if(!DImplementation->DoLeaveAt(Tokens)){
                return false;
            }
        }
        else if(Command == "arriveby"){
            if(!DImplementation->DoArriveBy(Tokens)){
                return false;
            }
        }
        else if(Command == "save"){
            if(!DImplementation->DoSave()){
                return false;
            }
        }
        else{
            std::ostringstream Err;
            Err << "Unknown command \"" << Command << "\" type help for help.\n";
            if(!DImplementation->WriteErr(Err.str())){
                return false;
            }
        }
    }
}