#include "SVGTripPlanWriter.h"

#include "BusSystemIndexer.h"
#include "StreetMapIndexer.h"

#include <algorithm>
#include <any>
#include <chrono>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

struct CSVGTripPlanWriter::SImplementation{
    struct SSVGConfig final : public CTripPlanWriter::SConfig{
        std::unordered_set<std::string> DEnabledFlags;
        std::unordered_map<std::string, std::any> DOptions;
        std::unordered_map<std::string, EOptionType> DOptionTypes;


        SSVGConfig(){
            DEnabledFlags.insert(std::string(CSVGTripPlanWriter::MotorwayEnabled));
            DEnabledFlags.insert(std::string(CSVGTripPlanWriter::PrimaryEnabled));
            DEnabledFlags.insert(std::string(CSVGTripPlanWriter::SecondaryEnabled));
            DEnabledFlags.insert(std::string(CSVGTripPlanWriter::TertiaryEnabled));
            DEnabledFlags.insert(std::string(CSVGTripPlanWriter::ResidentialEnabled));

            DOptionTypes[std::string(CSVGTripPlanWriter::SVGWidth)] = EOptionType::Int;
            DOptionTypes[std::string(CSVGTripPlanWriter::SVGHeight)] = EOptionType::Int;
            DOptionTypes[std::string(CSVGTripPlanWriter::SVGMarginPixels)] = EOptionType::Int;
            DOptionTypes[std::string(CSVGTripPlanWriter::SourceColor)] = EOptionType::String;
            DOptionTypes[std::string(CSVGTripPlanWriter::DestinationColor)] = EOptionType::String;
            DOptionTypes[std::string(CSVGTripPlanWriter::BusStopRadius)] = EOptionType::Double;
            DOptionTypes[std::string(CSVGTripPlanWriter::SourceRadius)] = EOptionType::Double;
            DOptionTypes[std::string(CSVGTripPlanWriter::DestinationRadius)] = EOptionType::Double;
            DOptionTypes[std::string(CSVGTripPlanWriter::BusColor0)] = EOptionType::String;
            DOptionTypes[std::string(CSVGTripPlanWriter::BusColor1)] = EOptionType::String;
            DOptionTypes[std::string(CSVGTripPlanWriter::StreetColor)] = EOptionType::String;
            DOptionTypes[std::string(CSVGTripPlanWriter::MotorwayStroke)] = EOptionType::Int;
            DOptionTypes[std::string(CSVGTripPlanWriter::PrimaryStroke)] = EOptionType::Int;
            DOptionTypes[std::string(CSVGTripPlanWriter::SecondaryStroke)] = EOptionType::Int;
            DOptionTypes[std::string(CSVGTripPlanWriter::TertiaryStroke)] = EOptionType::Int;
            DOptionTypes[std::string(CSVGTripPlanWriter::ResidentialStroke)] = EOptionType::Int;
            DOptionTypes[std::string(CSVGTripPlanWriter::BusStroke)] = EOptionType::Int;
            DOptionTypes[std::string(CSVGTripPlanWriter::LabelMargin)] = EOptionType::Int;
            DOptionTypes[std::string(CSVGTripPlanWriter::LabelColor)] = EOptionType::String;
            DOptionTypes[std::string(CSVGTripPlanWriter::LabelBackground)] = EOptionType::String;
            DOptionTypes[std::string(CSVGTripPlanWriter::LabelSize)] = EOptionType::Int;
            DOptionTypes[std::string(CSVGTripPlanWriter::LabelPaintOrder)] = EOptionType::String;

            DOptions[std::string(CSVGTripPlanWriter::BusColor0)] = std::string("#8E24AA");
            DOptions[std::string(CSVGTripPlanWriter::BusColor1)] = std::string("#F57C00");
            DOptions[std::string(CSVGTripPlanWriter::BusStopRadius)] = 8.0;
            DOptions[std::string(CSVGTripPlanWriter::BusStroke)] = 8;
            DOptions[std::string(CSVGTripPlanWriter::DestinationColor)] = std::string("#FF0000");
            DOptions[std::string(CSVGTripPlanWriter::DestinationRadius)] = 8.0;
            DOptions[std::string(CSVGTripPlanWriter::LabelBackground)] = std::string("#FFFFFF80");
            DOptions[std::string(CSVGTripPlanWriter::LabelColor)] = std::string("#000000");
            DOptions[std::string(CSVGTripPlanWriter::LabelMargin)] = 8;
            DOptions[std::string(CSVGTripPlanWriter::LabelPaintOrder)] = std::string("stroke fill");
            DOptions[std::string(CSVGTripPlanWriter::LabelSize)] = 16;
            DOptions[std::string(CSVGTripPlanWriter::MotorwayStroke)] = 6;
            DOptions[std::string(CSVGTripPlanWriter::PrimaryStroke)] = 4;
            DOptions[std::string(CSVGTripPlanWriter::ResidentialStroke)] = 2;
            DOptions[std::string(CSVGTripPlanWriter::SecondaryStroke)] = 2;
            DOptions[std::string(CSVGTripPlanWriter::SourceColor)] = std::string("#00FF00");
            DOptions[std::string(CSVGTripPlanWriter::SourceRadius)] = 8.0;
            DOptions[std::string(CSVGTripPlanWriter::StreetColor)] = std::string("#B0B0B0");
            DOptions[std::string(CSVGTripPlanWriter::SVGHeight)] = 540;
            DOptions[std::string(CSVGTripPlanWriter::SVGMarginPixels)] = 30;
            DOptions[std::string(CSVGTripPlanWriter::SVGWidth)] = 960;
            DOptions[std::string(CSVGTripPlanWriter::TertiaryStroke)] = 2;
        }

        bool FlagEnabled(std::string_view flag) const override{
            return DEnabledFlags.find(std::string(flag)) != DEnabledFlags.end();
        }

        void EnableFlag(std::string_view flag) override{
            auto Key = std::string(flag);
            auto Flags = ValidFlags();
            if(Flags.find(Key) != Flags.end()){
            DEnabledFlags.insert(Key);
            }
        }   

        void DisableFlag(std::string_view flag) override{
            DEnabledFlags.erase(std::string(flag));
        }

        std::any GetOption(std::string_view option) const override{
            auto It = DOptions.find(std::string(option));
            if(It == DOptions.end()){
                return std::any{};
            }
            return It->second;
        }

        std::unordered_set<std::string> ValidFlags() const override{
            return {
                std::string(CSVGTripPlanWriter::MotorwayEnabled),
                std::string(CSVGTripPlanWriter::PrimaryEnabled),
                std::string(CSVGTripPlanWriter::SecondaryEnabled),
                std::string(CSVGTripPlanWriter::TertiaryEnabled),
                std::string(CSVGTripPlanWriter::ResidentialEnabled)
            };
        }

        EOptionType GetOptionType(std::string_view option) const override{
            auto It = DOptionTypes.find(std::string(option));
            if(It == DOptionTypes.end()){
                return EOptionType::None;
            }
            return It->second;
        }

        void SetOption(std::string_view option, int value) override{
            auto Key = std::string(option);
            if(GetOptionType(Key) == EOptionType::Int){
                DOptions[Key] = value;
            }
        }

        void SetOption(std::string_view option, double value) override{
            auto Key = std::string(option);
            if(GetOptionType(Key) == EOptionType::Double){
                DOptions[Key] = value;
            }
        }

        void SetOption(std::string_view option, const std::string &value) override{
            auto Key = std::string(option);
            if(GetOptionType(Key) == EOptionType::String){
                DOptions[Key] = value;
            }
        }

        void ClearOption(std::string_view option) override{
            auto Key = std::string(option);
            if(DOptionTypes.find(Key) != DOptionTypes.end()){
                DOptions.erase(Key);
            }
        }

        std::unordered_set<std::string> ValidOptions() const override{
            std::unordered_set<std::string> Result;
            for(const auto &[Key,Value] : DOptionTypes){
                Result.insert(Key);
            }
            return Result;
        }
    };

    std::shared_ptr<CStreetMap> DStreetMap;
    std::shared_ptr<CBusSystem> DBusSystem;
    std::shared_ptr<CStreetMapIndexer> DStreetMapIndexer;
    std::shared_ptr<CBusSystemIndexer> DBusSystemIndexer;
    std::shared_ptr<SConfig> DConfig;

    
    double DMapMinLatitude = std::numeric_limits<double>::max();
    double DMapMaxLatitude = std::numeric_limits<double>::lowest();
    double DMapMinLongitude = std::numeric_limits<double>::max();
    double DMapMaxLongitude = std::numeric_limits<double>::lowest();

    std::unordered_map<CStreetMap::TNodeID, std::shared_ptr<CStreetMap::SNode>> DNodeByID;

    SImplementation(std::shared_ptr<CStreetMap> streetmap, std::shared_ptr<CBusSystem> bussystem)
        : DStreetMap(std::move(streetmap)),
          DBusSystem(std::move(bussystem)),
          DStreetMapIndexer(std::make_shared<CStreetMapIndexer>(DStreetMap)),
          DBusSystemIndexer(std::make_shared<CBusSystemIndexer>(DBusSystem)),
          DConfig(std::make_shared<SSVGConfig>()){

        if(DStreetMap){
            for(std::size_t Index = 0; Index < DStreetMap->NodeCount(); Index++){
                auto Node = DStreetMap->NodeByIndex(Index);
                if(Node){
                    DNodeByID[Node->ID()] = Node;

                    auto Loc = Node->Location();
                    DMapMinLatitude = std::min(DMapMinLatitude, Loc.DLatitude);
                    DMapMaxLatitude = std::max(DMapMaxLatitude, Loc.DLatitude);
                    DMapMinLongitude = std::min(DMapMinLongitude, Loc.DLongitude);
                    DMapMaxLongitude = std::max(DMapMaxLongitude, Loc.DLongitude);
                }
            }
        }
        
        if(DBusSystem){
            for(std::size_t Index = 0; Index < DBusSystem->StopCount(); Index++){
                auto Stop = DBusSystem->StopByIndex(Index);
                if(!Stop){
                    continue;
                }
                if(Stop->Description().empty()){
                    auto NewDescription = DefaultStopDescription(Stop->NodeID());
                    if(!NewDescription.empty()){
                        Stop->Description(NewDescription);
                    }
                }
            }
        }
    }

    std::shared_ptr<CStreetMap::SNode> NodeByID(CStreetMap::TNodeID nodeid) const{
        auto It = DNodeByID.find(nodeid);
        if(It == DNodeByID.end()){
            return nullptr;
        }
        return It->second;
    }

    std::shared_ptr<CBusSystem::SStop> StopByID(CBusSystem::TStopID stopid) const{
        return DBusSystem->StopByID(stopid);
    }

    std::string DefaultStopDescription(CStreetMap::TNodeID nodeid) const{

        std::vector<std::string> StreetNames;

        auto AddStreetName = [&](const std::string &Name){
            if(Name.empty()){
                return;
            }
            for(const auto &Existing : StreetNames){
                if(Existing == Name){
                    return;
                }
            }
            StreetNames.push_back(Name);
        };

        auto NodeWays = DStreetMapIndexer->WaysByNodeID(nodeid);

        //collect street names directly on this node.
        for(const auto &Way : NodeWays){
            if(!Way){
                continue;
            }
            AddStreetName(Way->GetAttribute("name"));
        }

        //if there is only one street name, inspect near nodes along connected ways.
        if(StreetNames.size() < 2){
            for(const auto &Way : NodeWays){
                if(!Way){
                    continue;
                }

                for(std::size_t Index = 0; Index < Way->NodeCount(); Index++){
                    if(Way->GetNodeID(Index) != nodeid){
                        continue;
                    }

                    if(Index > 0){
                        auto PrevNodeID = Way->GetNodeID(Index - 1);
                        auto PrevWays = DStreetMapIndexer->WaysByNodeID(PrevNodeID);
                        for(const auto &PrevWay : PrevWays){
                            if(PrevWay){
                                AddStreetName(PrevWay->GetAttribute("name"));
                            }
                        }
                    }

                    if(Index + 1 < Way->NodeCount()){
                        auto NextNodeID = Way->GetNodeID(Index + 1);
                        auto NextWays = DStreetMapIndexer->WaysByNodeID(NextNodeID);
                        for(const auto &NextWay : NextWays){
                            if(NextWay){
                                AddStreetName(NextWay->GetAttribute("name"));
                            }
                        }
                    }
                }
            }
        }

        if(StreetNames.size() >= 2){
            return StreetNames[0] + " & " + StreetNames[1];
        }
        if(StreetNames.size() == 1){
            return StreetNames[0];
        }
        return "";
    }
    

    static bool WriteString(std::shared_ptr<CDataSink> sink, const std::string &str){
        std::vector<char> Buffer(str.begin(), str.end());
        return sink->Write(Buffer);
    }

    static std::string Escape(const std::string &input){
        std::string Result;
        for(char Ch : input){
            switch(Ch){
                case '&': Result += "&amp;"; break;
                case '<': Result += "&lt;"; break;
                case '>': Result += "&gt;"; break;
                case '"': Result += "&quot;"; break;
                case '\'': Result += "&apos;"; break;
                default: Result += Ch; break;
            }
        }
        return Result;
    }

    bool WayEnabled(const std::shared_ptr<CStreetMap::SWay> &Way) const{
        if(!Way){
            return false;
        }

        auto Highway = Way->GetAttribute("highway");

        if(Highway == "motorway"){
            return DConfig->FlagEnabled(CSVGTripPlanWriter::MotorwayEnabled);
        }
        if(Highway == "primary"){
            return DConfig->FlagEnabled(CSVGTripPlanWriter::PrimaryEnabled);
        }
        if(Highway == "secondary"){
            return DConfig->FlagEnabled(CSVGTripPlanWriter::SecondaryEnabled);
        }
        if(Highway == "tertiary"){
            return DConfig->FlagEnabled(CSVGTripPlanWriter::TertiaryEnabled);
        }
        if(Highway == "residential"){
            return DConfig->FlagEnabled(CSVGTripPlanWriter::ResidentialEnabled);
        }
        return false;
    }

    int WayStroke(const std::shared_ptr<CStreetMap::SWay> &Way) const{
        if(!Way){
            return 1;
        }

        auto Highway = Way->GetAttribute("highway");

        if(Highway == "motorway"){
            return std::any_cast<int>(DConfig->GetOption(CSVGTripPlanWriter::MotorwayStroke));
        }
        if(Highway == "primary"){
            return std::any_cast<int>(DConfig->GetOption(CSVGTripPlanWriter::PrimaryStroke));
        }
        if(Highway == "secondary"){
            return std::any_cast<int>(DConfig->GetOption(CSVGTripPlanWriter::SecondaryStroke));
        }
        if(Highway == "tertiary"){
            return std::any_cast<int>(DConfig->GetOption(CSVGTripPlanWriter::TertiaryStroke));
        }
        if(Highway == "residential"){
            return std::any_cast<int>(DConfig->GetOption(CSVGTripPlanWriter::ResidentialStroke));
        }
        return 1;
    }
};

namespace {
    struct SProjectedPoint{
        double X;
        double Y;
    };
}

CSVGTripPlanWriter::CSVGTripPlanWriter(std::shared_ptr<CStreetMap> streetmap, std::shared_ptr<CBusSystem> bussystem)
    : DImplementation(std::make_unique<SImplementation>(std::move(streetmap), std::move(bussystem))){
}

CSVGTripPlanWriter::~CSVGTripPlanWriter() = default;

std::shared_ptr<CTripPlanWriter::SConfig> CSVGTripPlanWriter::Config() const{
    return DImplementation->DConfig;
}

bool CSVGTripPlanWriter::WritePlan(std::shared_ptr<CDataSink> sink, const TTravelPlan &plan){
    if(!sink || plan.size() < 2){
        return false;
    }

    using TNodeID = CStreetMap::TNodeID;
    using TStopID = CBusSystem::TStopID;

    struct SBusLeg{
        std::string DRouteName;
        std::shared_ptr<CBusSystem::SPath> DPath;
        std::vector<TStopID> DDisplayedStops;
        std::size_t DPlanLegIndex = 0;
    };

    std::vector<SBusLeg> Legs;
    std::vector<TNodeID> AllPathNodes;
    std::vector<TStopID> OrderedUniqueStops;
    std::unordered_set<TStopID> SeenStops;

    auto AddStopUnique = [&](TStopID stopid){
    if(SeenStops.find(stopid) == SeenStops.end()){
        SeenStops.insert(stopid);
        OrderedUniqueStops.push_back(stopid);
    }
    };

    for(std::size_t StepIndex = 0; StepIndex + 1 < plan.size(); StepIndex++){
    const auto &Current = plan[StepIndex];
    const auto &Next = plan[StepIndex + 1];

    auto Route = DImplementation->DBusSystemIndexer->RouteByName(Current.DRouteName);
    if(!Route){
        return false;
    }

    auto StopIDs = Route->StopIDsSourceDestination(Current.DStopID, Next.DStopID);
    if(StopIDs.size() < 2){
        return false;
    }

    for(auto StopID : StopIDs){
        AddStopUnique(StopID);
    }

    for(std::size_t StopIndex = 0; StopIndex + 1 < StopIDs.size(); StopIndex++){
        auto Path = DImplementation->DBusSystem->PathByStopIDs(StopIDs[StopIndex], StopIDs[StopIndex + 1]);
        if(!Path){
            return false;
        }

        SBusLeg Leg;
        Leg.DRouteName = Current.DRouteName;
        Leg.DPath = Path;
        Leg.DDisplayedStops = {StopIDs[StopIndex], StopIDs[StopIndex + 1]};
        Leg.DPlanLegIndex = StepIndex;
        Legs.push_back(Leg);

        for(std::size_t NodeIndex = 0; NodeIndex < Path->NodeCount(); NodeIndex++){
            AllPathNodes.push_back(Path->GetNodeID(NodeIndex));
        }
    }
}

    if(AllPathNodes.empty() || OrderedUniqueStops.empty()){
        return false;
    }

    double MinLat = std::numeric_limits<double>::max();
    double MaxLat = std::numeric_limits<double>::lowest();
    double MinLon = std::numeric_limits<double>::max();
    double MaxLon = std::numeric_limits<double>::lowest();
    double StopMinLat = std::numeric_limits<double>::max();
    double StopMaxLat = std::numeric_limits<double>::lowest();

    auto ConsiderNode = [&](TNodeID nodeid){
        auto Node = DImplementation->NodeByID(nodeid);
        if(!Node){
            return;
        }
        auto Loc = Node->Location();
        MinLat = std::min(MinLat, Loc.DLatitude);
        MaxLat = std::max(MaxLat, Loc.DLatitude);
        MinLon = std::min(MinLon, Loc.DLongitude);
        MaxLon = std::max(MaxLon, Loc.DLongitude);
    };

    for(auto NodeID : AllPathNodes){
        ConsiderNode(NodeID);
    }

    for(auto StopID : OrderedUniqueStops){
        auto Stop = DImplementation->StopByID(StopID);
        if(!Stop){
            return false;
        }
        ConsiderNode(Stop->NodeID());

        auto StopNode = DImplementation->NodeByID(Stop->NodeID());
        if(StopNode){
            auto Loc = StopNode->Location();
            StopMinLat = std::min(StopMinLat, Loc.DLatitude);
            StopMaxLat = std::max(StopMaxLat, Loc.DLatitude);
        }
    }

    if(!(MinLat <= MaxLat) || !(MinLon <= MaxLon)){
        return false;
    }

    int SVGWidth = std::any_cast<int>(DImplementation->DConfig->GetOption(CSVGTripPlanWriter::SVGWidth));
    int SVGHeight = std::any_cast<int>(DImplementation->DConfig->GetOption(CSVGTripPlanWriter::SVGHeight));
    int Margin = std::any_cast<int>(DImplementation->DConfig->GetOption(CSVGTripPlanWriter::SVGMarginPixels));

    double LatRange = MaxLat - MinLat;
    double LonRange = MaxLon - MinLon;

    if(LatRange == 0.0){
        LatRange = 1e-6;
        MinLat -= LatRange / 2.0;
        MaxLat += LatRange / 2.0;
    }
    if(LonRange == 0.0){
        LonRange = 1e-6;
        MinLon -= LonRange / 2.0;
        MaxLon += LonRange / 2.0;
    }

    //add a fixed geographic buffer around the travel area
    //match the  latitude/longitude degree change
    double ExpandLat = 0.25 / 69.0;
    double ExpandLon = 0.25 / 54.2;

    CStreetMap::SLocation LowerLeft{MinLat - ExpandLat, MinLon - ExpandLon};
    CStreetMap::SLocation UpperRight{MaxLat + ExpandLat, MaxLon + ExpandLon};

    auto Project = [&](const CStreetMap::SLocation &Loc) -> SProjectedPoint {
        double X = Margin + (Loc.DLongitude - LowerLeft.DLongitude) *
                            static_cast<double>(SVGWidth - 2 * Margin) /
                            (UpperRight.DLongitude - LowerLeft.DLongitude);

        double Y = SVGHeight - Margin - (Loc.DLatitude - LowerLeft.DLatitude) *
                                      static_cast<double>(SVGHeight - 2 * Margin) /
                                      (UpperRight.DLatitude - LowerLeft.DLatitude);
        return {X, Y};
    };

    std::ostringstream Out;
    Out << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" << SVGWidth
        << "\" height=\"" << SVGHeight << "\">\n";

    //draw streets
    CStreetMap::SLocation StreetLowerLeft{StopMinLat, DImplementation->DMapMinLongitude};
    CStreetMap::SLocation StreetUpperRight{StopMaxLat, DImplementation->DMapMaxLongitude};
    auto Ways = DImplementation->DStreetMapIndexer->WaysInRange(StreetLowerLeft, StreetUpperRight);
    auto StreetColor = std::any_cast<std::string>(DImplementation->DConfig->GetOption(CSVGTripPlanWriter::StreetColor));

    for(const auto &Way : Ways){
        if(!DImplementation->WayEnabled(Way)){
            continue;
        }
        if(Way->NodeCount() < 2){
            continue;
        }

        int Stroke = DImplementation->WayStroke(Way);

        for(std::size_t NodeIndex = 0; NodeIndex + 1 < Way->NodeCount(); NodeIndex++){
            auto NodeA = DImplementation->NodeByID(Way->GetNodeID(NodeIndex));
            auto NodeB = DImplementation->NodeByID(Way->GetNodeID(NodeIndex + 1));
            if(!NodeA || !NodeB){
                continue;
            }

            auto PA = Project(NodeA->Location());
            auto PB = Project(NodeB->Location());

            Out << "<line x1=\"" << PA.X << "\" y1=\"" << PA.Y
                << "\" x2=\"" << PB.X << "\" y2=\"" << PB.Y
                << "\" style=\"stroke:" << StreetColor
                << ";stroke-width:" << Stroke << "\" />\n";
        }
    }

    //draw bus paths
    auto BusColor0Value = std::any_cast<std::string>(DImplementation->DConfig->GetOption(CSVGTripPlanWriter::BusColor0));
    auto BusColor1Value = std::any_cast<std::string>(DImplementation->DConfig->GetOption(CSVGTripPlanWriter::BusColor1));
    int BusStrokeValue = std::any_cast<int>(DImplementation->DConfig->GetOption(CSVGTripPlanWriter::BusStroke));

    for(std::size_t LegIndex = 0; LegIndex < Legs.size(); LegIndex++){
        const auto &Leg = Legs[LegIndex];
        auto BusColor = (Leg.DPlanLegIndex % 2 == 0) ? BusColor0Value : BusColor1Value;

        if(!Leg.DPath || Leg.DPath->NodeCount() < 2){
            continue;
        }

        for(std::size_t NodeIndex = 0; NodeIndex + 1 < Leg.DPath->NodeCount(); NodeIndex++){
            auto NodeA = DImplementation->NodeByID(Leg.DPath->GetNodeID(NodeIndex));
            auto NodeB = DImplementation->NodeByID(Leg.DPath->GetNodeID(NodeIndex + 1));
            if(!NodeA || !NodeB){
                continue;
            }

            auto PA = Project(NodeA->Location());
            auto PB = Project(NodeB->Location());

            Out << "<line x1=\"" << PA.X << "\" y1=\"" << PA.Y
                << "\" x2=\"" << PB.X << "\" y2=\"" << PB.Y
                << "\" style=\"stroke:" << BusColor
                << ";stroke-width:" << BusStrokeValue << "\" />\n";
        }
    }

    //draw circles and labels for displayed stops
    double SourceRadiusValue = std::any_cast<double>(DImplementation->DConfig->GetOption(CSVGTripPlanWriter::SourceRadius));
    double DestinationRadiusValue = std::any_cast<double>(DImplementation->DConfig->GetOption(CSVGTripPlanWriter::DestinationRadius));
    double BusStopRadiusValue = std::any_cast<double>(DImplementation->DConfig->GetOption(CSVGTripPlanWriter::BusStopRadius));
    auto SourceColorValue = std::any_cast<std::string>(DImplementation->DConfig->GetOption(CSVGTripPlanWriter::SourceColor));
    auto DestinationColorValue = std::any_cast<std::string>(DImplementation->DConfig->GetOption(CSVGTripPlanWriter::DestinationColor));
    auto LabelColorValue = std::any_cast<std::string>(DImplementation->DConfig->GetOption(CSVGTripPlanWriter::LabelColor));
    auto LabelBackgroundValue = std::any_cast<std::string>(DImplementation->DConfig->GetOption(CSVGTripPlanWriter::LabelBackground));
    auto LabelPaintOrderValue = std::any_cast<std::string>(DImplementation->DConfig->GetOption(CSVGTripPlanWriter::LabelPaintOrder));
    int LabelMarginValue = std::any_cast<int>(DImplementation->DConfig->GetOption(CSVGTripPlanWriter::LabelMargin));
    int LabelSizeValue = std::any_cast<int>(DImplementation->DConfig->GetOption(CSVGTripPlanWriter::LabelSize));

    for(std::size_t StopIndex = 0; StopIndex < OrderedUniqueStops.size(); StopIndex++){
        auto Stop = DImplementation->StopByID(OrderedUniqueStops[StopIndex]);
        if(!Stop){
            return false;
        }
        auto Node = DImplementation->NodeByID(Stop->NodeID());
        if(!Node){
            return false;
        }

        auto P = Project(Node->Location());

        double Radius = BusStopRadiusValue;
        std::string Fill = BusColor0Value;

        if(StopIndex == 0){
            Radius = SourceRadiusValue;
            Fill = SourceColorValue;
        }
        else if(StopIndex + 1 == OrderedUniqueStops.size()){
            Radius = DestinationRadiusValue;
            Fill = DestinationColorValue;
        }

        Out << "<circle cx=\"" << P.X << "\" cy=\"" << P.Y
            << "\" r=\"" << Radius
            << "\" style=\"fill:" << Fill << "\" />\n";

        Out << "<text x=\"" << (P.X + LabelMarginValue)
            << "\" y=\"" << (P.Y - LabelMarginValue)
            << "\" style=\"fill:" << LabelColorValue
            << ";stroke:" << LabelBackgroundValue
            << ";paint-order:" << LabelPaintOrderValue
            << ";font-size:" << LabelSizeValue << "\">"
            << SImplementation::Escape(Stop->Description())
            << "</text>\n";
    }

    Out << "</svg>\n";

    return SImplementation::WriteString(sink, Out.str());
}