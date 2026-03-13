#include "XMLBusSystem.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <cctype>
using std::cout;
using std::endl;


//small helper to has an std::pair for the unordered map
//hash function so we can use NodeID as a key
struct SPairHash{
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1,T2> &p) const noexcept{
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1 << 6U) + (h1 >> 2U));
    }
};



struct CXMLBusSystem::SImplementation{

    //tag attribute names
    const std::string DBusSystemTag = "bussystem";
    const std::string DStopsTag = "stops";
    const std::string DStopTag = "stop";
    const std::string DStopIDAttr = "id";
    const std::string DStopNodeAttr = "node";
    const std::string DStopDescAttr = "description";

    const std::string DRoutesTag = "routes";
    const std::string DRouteTag = "route";
    const std::string DRouteNameAttr = "name";
    const std::string DRouteStopTag = "routestop";
    const std::string DRouteStopAttr = "stop";
    const std::string DRouteScheduleAttr = "schedule";
    const std::string DRouteDeltaAttr = "delta";

    const std::string DPathsTag = "paths";
    const std::string DPathTag = "path";
    const std::string DPathSrcAttr = "source";
    const std::string DPathDstAttr = "destination";
    const std::string DPathNodeTag = "node";
    const std::string DPathNodeIDAttr = "id";


    //stop implementation
    //represents a bus stop
    /*
    ID retunrs stop id
    NodeID returns street map node id
    Description return current description
    Description(newDesc) sets description and returns it
    */
    struct SStop : public CBusSystem::SStop{
        TStopID DID;
        CStreetMap::TNodeID DNodeID;
        std::string DDescription;

        SStop(TStopID id, CStreetMap::TNodeID nodeid, const std::string &description){
            DID = id;
            DNodeID = nodeid;
            DDescription = description;
        }
        ~SStop(){};
        TStopID ID() const noexcept override{
            return DID;
        }

        CStreetMap::TNodeID NodeID() const noexcept override{
                return DNodeID;
        }

        std::string Description() const noexcept override{
            return DDescription;
        }

        std::string Description(const std::string &description) noexcept override{
            DDescription = description;
            return DDescription;
        }
    };


    //route implementation
    //represnets a bus route
    //stores route nanme and ordered list of stop ids
    //name, stopcount, getstopid by interface
    struct SRoute : public CBusSystem::SRoute{
        std::vector<TStopID> DStopIDs;
        std::vector<TStopTime> DStartTimes;
        std::vector<double> DStopDeltas;
        std::string DName;

    explicit SRoute(const std::string &name) : DName(name){
    }

    ~SRoute() override = default;

    std::string Name() const noexcept override{
        return DName;
    }

    std::size_t StopCount() const noexcept override{
        return DStopIDs.size();
    }

    TStopID GetStopID(std::size_t index) const noexcept override{
        if(index >= DStopIDs.size()){
            return CBusSystem::InvalidStopID;
        }
        return DStopIDs[index];
    }

    std::size_t TripCount() const noexcept override{
        return DStartTimes.size();
    }

    TStopTime GetStopTime(std::size_t stopindex, std::size_t tripindex) const noexcept override{
        if((tripindex >= DStartTimes.size()) || (stopindex >= DStopDeltas.size())){
            return TStopTime();
        }
        return TStopTime(
            DStartTimes[tripindex].to_duration() +
            std::chrono::minutes(static_cast<int>(DStopDeltas[stopindex]))
        );
    }
};


    //path implementation
    //represents a path between two street nodes
    //stores start and end node ids and ordered list
    struct SPath : public CBusSystem::SPath{
        CStreetMap::TNodeID DStart{};
        CStreetMap::TNodeID DEnd{};
        std::vector<CStreetMap::TNodeID> DNodeIDs;

        SPath(CStreetMap::TNodeID start, CStreetMap::TNodeID end)
            : DStart(start), DEnd(end){
        }

        ~SPath() override = default;

        CStreetMap::TNodeID StartNodeID() const noexcept override{
            return DStart;
        }

        CStreetMap::TNodeID EndNodeID() const noexcept override{
            return DEnd;
        }

        std::size_t NodeCount() const noexcept override{
            return DNodeIDs.size();
        }

        CStreetMap::TNodeID GetNodeID(std::size_t index) const noexcept override{
            if(index >= DNodeIDs.size()){
                return CStreetMap::InvalidNodeID;
            }
            return DNodeIDs[index];
        }
    };



//helpers start and end
//scans forwards until it finds start tag
    bool FindStartTag(std::shared_ptr< CXMLReader > xmlsource, const std::string &starttag){
        SXMLEntity TempEntity;
        while(xmlsource->ReadEntity(TempEntity,true)){
            if((TempEntity.DType == SXMLEntity::EType::StartElement)&&(TempEntity.DNameData == starttag)){
                return true;
            }
        }
        return false;
    }

    //scans forward until it finds end tag
    bool FindEndTag(std::shared_ptr< CXMLReader > xmlsource, const std::string &starttag){
        SXMLEntity TempEntity;
        while(xmlsource->ReadEntity(TempEntity,true)){
            if((TempEntity.DType == SXMLEntity::EType::EndElement)&&(TempEntity.DNameData == starttag)){
                return true;
            }
        }
        return false;
    }

    //helper to trim the routename
    //removes leading whitespaces and normalizes route names
    static std::string Trim(const std::string &str){
        std::size_t start = 0;
        while(start < str.size() && std::isspace(static_cast<unsigned char>(str[start]))){
            start++;
        }

        std::size_t end = str.size();
        while(end > start && std::isspace(static_cast<unsigned char>(str[end - 1]))){
            end--;
        }

        return str.substr(start, end - start);
    }

    static TStopTime ParseStopTimeString(const std::string &str){
        auto Clean = Trim(str);

        std::size_t ColonPos = Clean.find(':');
        std::size_t SpacePos = Clean.find(' ', ColonPos);

        if(ColonPos == std::string::npos || SpacePos == std::string::npos){
            return TStopTime();
        }

        int Hour = std::stoi(Clean.substr(0, ColonPos));
        int Minute = std::stoi(Clean.substr(ColonPos + 1, SpacePos - ColonPos - 1));
        std::string Suffix = Clean.substr(SpacePos + 1);

        for(auto &Ch : Suffix){
            Ch = std::toupper(static_cast<unsigned char>(Ch));
        }

        if(Suffix == "PM" && Hour != 12){
            Hour += 12;
        }
        else if(Suffix == "AM" && Hour == 12){
            Hour = 0;
        }

        return TStopTime(std::chrono::hours(Hour) + std::chrono::minutes(Minute));
    }


    //storage containers to keep track of the objects
    std::vector<std::shared_ptr<SStop> > DStopsByIndex;
    std::unordered_map<TStopID,std::shared_ptr<SStop> > DStopsByID;
    std::vector<std::shared_ptr<SRoute>> DRoutesByIndex;
    std::unordered_map<std::string, std::shared_ptr<SRoute>> DRoutesByName;
    std::unordered_map<std::pair<CStreetMap::TNodeID, CStreetMap::TNodeID>, std::shared_ptr<SPath>, SPairHash> DPathsByNodeIDs;

    //reads attributes from stop and stores a new SStop
    void ParseStop(std::shared_ptr< CXMLReader > systemsource, const SXMLEntity &stop){
        TStopID StopID = std::stoull(stop.AttributeValue(DStopIDAttr));
        CStreetMap::TNodeID NodeID = std::stoull(stop.AttributeValue(DStopNodeAttr));
        auto NewStop = std::make_shared<SStop>(StopID, NodeID, stop.AttributeValue(DStopDescAttr));
        DStopsByIndex.push_back(NewStop);
        DStopsByID[StopID] = NewStop;
    }

    //reads everything inside stops and calls ParseStop for each stop
    void ParseStops(const std::shared_ptr< CXMLReader > &systemsource){
        SXMLEntity tempEntity;

        while(systemsource->ReadEntity(tempEntity, true)){
            if(tempEntity.DType == SXMLEntity::EType::StartElement && tempEntity.DNameData == DStopTag){
                ParseStop(systemsource, tempEntity);
            }
            else if(tempEntity.DType == SXMLEntity::EType::EndElement && tempEntity.DNameData == DStopsTag){
                break;
            }
        }
    }

    //reads route name and scans inside the route tag for stop entries.
    void ParseRoute(const std::shared_ptr<CXMLReader> &systemsource, const SXMLEntity &routeEntity){
        std::string routeName = routeEntity.AttributeValue(DRouteNameAttr);

        // some xml files may use a different attribute name for route name
        if(routeName.empty()){
            routeName = routeEntity.AttributeValue("id");
        }
        if(routeName.empty()){
            routeName = routeEntity.AttributeValue("route");
        }

        // fix the key
        routeName = Trim(routeName);



    // If the route has no name, we ignore it, but we must read until </route>
    if(routeName.empty()){
        SXMLEntity e;
        while(systemsource->ReadEntity(e, true)){
            if(e.DType == SXMLEntity::EType::EndElement && e.DNameData == DRouteTag){
                break;
            }
        }


        return;
    }

    auto newRoute = std::make_shared<SRoute>(routeName);

    std::string ScheduleStr = routeEntity.AttributeValue(DRouteScheduleAttr);
    if(!ScheduleStr.empty()){
        std::size_t Start = 0;
        while(Start < ScheduleStr.size()){
            std::size_t CommaPos = ScheduleStr.find(',', Start);
            std::string TimeToken;

            if(CommaPos == std::string::npos){
                TimeToken = ScheduleStr.substr(Start);
                Start = ScheduleStr.size();
            }
            else{
                TimeToken = ScheduleStr.substr(Start, CommaPos - Start);
                Start = CommaPos + 1;
            }

            TimeToken = Trim(TimeToken);
            if(!TimeToken.empty()){
                newRoute->DStartTimes.push_back(ParseStopTimeString(TimeToken));
            }
        }
    }

    // Read elements inside <route> only care about <routestop stop="..."/>
    SXMLEntity tempEntity;
    while(systemsource->ReadEntity(tempEntity, true)){
        if(tempEntity.DType == SXMLEntity::EType::StartElement && tempEntity.DNameData == DRouteStopTag){
            const std::string StopStr = tempEntity.AttributeValue(DRouteStopAttr);
            if(!StopStr.empty()){
                newRoute->DStopIDs.push_back(std::stoull(StopStr));
            }

            const std::string DeltaStr = tempEntity.AttributeValue(DRouteDeltaAttr);
            if(!DeltaStr.empty()){
                newRoute->DStopDeltas.push_back(std::stod(DeltaStr));
            }
            else{
                newRoute->DStopDeltas.push_back(0.0);
            }
        }
        else if(tempEntity.DType == SXMLEntity::EType::EndElement && tempEntity.DNameData == DRouteTag){
            break;
        }
    }


DRoutesByIndex.push_back(newRoute);
DRoutesByName[routeName] = newRoute;

}

//reads iunside route and calls parseroute for each <route>
void ParseRoutes(const std::shared_ptr<CXMLReader> &systemsource){
    SXMLEntity tempEntity;
    while(systemsource->ReadEntity(tempEntity, true)){
        if(tempEntity.DType == SXMLEntity::EType::StartElement && tempEntity.DNameData == DRouteTag){
            ParseRoute(systemsource, tempEntity);
        }
        else if(tempEntity.DType == SXMLEntity::EType::EndElement && tempEntity.DNameData == DRoutesTag){
            break;
        }
    }
}

    
    //parse bus system
    //find bussystem tag and locatres the stops and routes and parses them
    void ParseBusSystem(std::shared_ptr<CXMLReader> systemsource){
        if(!FindStartTag(systemsource, DBusSystemTag)){
            return ;
        }

        SXMLEntity TempEntity;
        while(systemsource->ReadEntity(TempEntity, true)){
            if((TempEntity.DType == SXMLEntity::EType::StartElement) && (TempEntity.DNameData == DStopsTag)){
                ParseStops(systemsource);
            }
            else if((TempEntity.DType == SXMLEntity::EType::StartElement) && (TempEntity.DNameData == DRoutesTag)){
                ParseRoutes(systemsource);
            }
            else if((TempEntity.DType == SXMLEntity::EType::EndElement) && (TempEntity.DNameData == DBusSystemTag)){
                break;
            }
        }
    }



    //parse paths
    //reads the source and destination attributes and then reads the node ids entries inside of the path
    void ParsePath(std::shared_ptr<CXMLReader> pathsource, const SXMLEntity &PathEntity){
        std::string SrcStr = PathEntity.AttributeValue(DPathSrcAttr);
        std::string DstStr = PathEntity.AttributeValue(DPathDstAttr);

        if(SrcStr.empty() || DstStr.empty()){
            // consume until </path>
            SXMLEntity TempEntity;
            while(pathsource->ReadEntity(TempEntity, true)){
                if((TempEntity.DType == SXMLEntity::EType::EndElement) && (TempEntity.DNameData == DPathTag)){
                    break;
                }
            }
            return;
        }

        CStreetMap::TNodeID StartNode = std::stoull(SrcStr);
        CStreetMap::TNodeID EndNode   = std::stoull(DstStr);

        auto NewPath = std::make_shared<SPath>(StartNode, EndNode);

        SXMLEntity TempEntity;
        while(pathsource->ReadEntity(TempEntity, true)){
            if((TempEntity.DType == SXMLEntity::EType::StartElement) && (TempEntity.DNameData == DPathNodeTag)){
                std::string NodeStr = TempEntity.AttributeValue(DPathNodeIDAttr);
                if(!NodeStr.empty()){
                    NewPath->DNodeIDs.push_back(std::stoull(NodeStr));
                }
            }
            else if((TempEntity.DType == SXMLEntity::EType::EndElement) && (TempEntity.DNameData == DPathTag)){
                break;
            }
        }

        DPathsByNodeIDs[{StartNode, EndNode}] = NewPath;
    }

    //find <paths> and parse all the path entries
    void ParsePaths(std::shared_ptr<CXMLReader> pathsource){
        if(!FindStartTag(pathsource, DPathsTag)){
            return;
        }

        SXMLEntity TempEntity;
        while(pathsource->ReadEntity(TempEntity, true)){
            if((TempEntity.DType == SXMLEntity::EType::StartElement) && (TempEntity.DNameData == DPathTag)){
                ParsePath(pathsource, TempEntity);
            }
            else if((TempEntity.DType == SXMLEntity::EType::EndElement) && (TempEntity.DNameData == DPathsTag)){
                break;
            }
        }
    }

    SImplementation(std::shared_ptr< CXMLReader > systemsource, std::shared_ptr< CXMLReader > pathsource){
        ParseBusSystem(systemsource);
        ParsePaths(pathsource);
        
    }

    std::size_t StopCount() const noexcept{
        return DStopsByIndex.size();
    }

    std::size_t RouteCount() const noexcept{
        return DRoutesByIndex.size();    }
    
    std::shared_ptr<SStop> StopByIndex(std::size_t index) const noexcept{
        if(index >= DStopsByIndex.size()){
            return nullptr;
        }
        return DStopsByIndex[index];
    }
    
    std::shared_ptr<SStop> StopByID(TStopID id) const noexcept{
        auto it = DStopsByID.find(id);
        if(it == DStopsByID.end()){
            return nullptr;
        }
        return it->second;
    }
    
    std::shared_ptr<SRoute> RouteByIndex(std::size_t index) const noexcept{
        if(index >= DRoutesByIndex.size()){
            return nullptr;
        }
        return DRoutesByIndex[index];
    }
    
    std::shared_ptr<SRoute> RouteByName(const std::string &name) const noexcept{
        auto it = DRoutesByName.find(Trim(name));
        if(it == DRoutesByName.end()){
            return nullptr;
        }
        return it->second;
    }
    
    std::shared_ptr<SPath> PathByStopIDs(TStopID start, TStopID end) const noexcept{
        auto StartStop = StopByID(start);
        auto EndStop   = StopByID(end);

        if(!StartStop || !EndStop){
            return nullptr;
        }

        CStreetMap::TNodeID StartNode = StartStop->NodeID();
        CStreetMap::TNodeID EndNode   = EndStop->NodeID();

        auto Iter = DPathsByNodeIDs.find({StartNode, EndNode});
        if(Iter == DPathsByNodeIDs.end()){
            return nullptr;
        }
        return Iter->second;
    }
    
};

CXMLBusSystem::CXMLBusSystem(std::shared_ptr< CXMLReader > systemsource, std::shared_ptr< CXMLReader > pathsource){
    DImplementation = std::make_unique<SImplementation>(systemsource,pathsource);
}
    
CXMLBusSystem::~CXMLBusSystem(){

}
    
std::size_t CXMLBusSystem::StopCount() const noexcept{
    return DImplementation->StopCount();
}
    
std::size_t CXMLBusSystem::RouteCount() const noexcept{
    return DImplementation->RouteCount();
}

std::shared_ptr<CBusSystem::SStop> CXMLBusSystem::StopByIndex(std::size_t index) const noexcept{
    return DImplementation->StopByIndex(index);
}

std::shared_ptr<CBusSystem::SStop> CXMLBusSystem::StopByID(TStopID id) const noexcept{
    return DImplementation->StopByID(id);
}

std::shared_ptr<CBusSystem::SRoute> CXMLBusSystem::RouteByIndex(std::size_t index) const noexcept{
    return DImplementation->RouteByIndex(index);
}

std::shared_ptr<CBusSystem::SRoute> CXMLBusSystem::RouteByName(const std::string &name) const noexcept{
    return DImplementation->RouteByName(name);
}

std::shared_ptr<CBusSystem::SPath> CXMLBusSystem::PathByStopIDs(TStopID start, TStopID end) const noexcept{
    return DImplementation->PathByStopIDs(start, end);
}

