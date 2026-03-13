#include "OpenStreetMap.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>

/*
implements COpenstreetmap to parse an openstreetmap XML document using CXMLReader and stores nodes and ways;
each node and way may also contain tag data which is stored as key and value attribtes
*/


struct COpenStreetMap::SImplementation{

    //xml tag and attribute names
    const std::string DOSMTag = "osm";
    const std::string DNodeTag = "node";
    const std::string DWayTag = "way";
    const std::string DTagTag  = "tag";
    const std::string DTagKeyAttr = "k";
    const std::string DTagValueAttr = "v";
    const std::string DNDTag = "nd";
    const std::string DNDRefAttr = "ref";


    struct SNode: public CStreetMap::SNode{
        const std::string DNodeIDAttr = "id";
        const std::string DNodeLatAttr = "lat";
        const std::string DNodeLonAttr = "lon";
        TNodeID DID;
        SLocation DLocation;

        //attributes in containers
            //vector for index ordering (getattributekey)
            // a map for quick lookup
        std::vector<std::string> DAttributeKeys;
        std::unordered_map<std::string,std::string> DAttributes;

        //constructs a node from the <node> start entity

        SNode(const SXMLEntity &entity){
            auto NodeID = std::stoull(entity.AttributeValue(DNodeIDAttr));
            auto NodeLat = std::stod(entity.AttributeValue(DNodeLatAttr));
            auto NodeLon = std::stod(entity.AttributeValue(DNodeLonAttr));
            DID = NodeID;
            DLocation = SLocation{NodeLat,NodeLon};
            
        }
        ~SNode(){

        }

        TNodeID ID() const noexcept override{
            return DID;
        }
        
        SLocation Location() const noexcept override{
            return DLocation;
        }
        
        std::size_t AttributeCount() const noexcept override{
            return DAttributeKeys.size();
        }
        
        std::string GetAttributeKey(std::size_t index) const noexcept override{
            if(index >= DAttributeKeys.size()){
                return "";
            }
            return DAttributeKeys[index];
        }
        
        bool HasAttribute(const std::string &key) const noexcept override{

            return DAttributes.find(key) != DAttributes.end();
            
        }
        
        std::string GetAttribute(const std::string &key) const noexcept override{
            auto It = DAttributes.find(key);
            if(It == DAttributes.end()){
                return "";
            }
            return It->second;
        }

        //helper used to store tag data to make sure that we only push the key into DAttributekeys once but can overwrite
        void SetAttribute(const std::string &key, const std::string &value){
            // Only add to key list the first time we see the key
            if(DAttributes.find(key) == DAttributes.end()){
                DAttributeKeys.push_back(key);
            }
            DAttributes[key] = value;
        }
        
    };

    struct SWay: public CStreetMap::SWay{
        
        const std::string DWayIDAttr = "id";

        TWayID DID = 0;
        std::vector<TNodeID> DNodeIDs;

        std::vector<std::string> DAttributeKeys;
        std::unordered_map<std::string,std::string> DAttributes;


        //constructs a way from the <way> start tag

        SWay(const SXMLEntity &entity){
            DID = std::stoull(entity.AttributeValue(DWayIDAttr));
        }


        ~SWay() override = default;

        TWayID ID() const noexcept override{
            return DID;
        }
        
        std::size_t NodeCount() const noexcept override{
            return DNodeIDs.size();
        }
        
        TNodeID GetNodeID(std::size_t index) const noexcept override{
            if(index >= DNodeIDs.size()){
                return InvalidNodeID;
            }
            return DNodeIDs[index];
        }
        
        std::size_t AttributeCount() const noexcept override{
            return DAttributeKeys.size();
        }
        
        std::string GetAttributeKey(std::size_t index) const noexcept override{
            if(index >= DAttributeKeys.size()){
                return "";
            }
            return DAttributeKeys[index];
        }
        
        bool HasAttribute(const std::string &key) const noexcept override{
            return DAttributes.find(key) != DAttributes.end();
        }
        
        std::string GetAttribute(const std::string &key) const noexcept override{
            auto It = DAttributes.find(key);
            if(It == DAttributes.end()){
                return "";
            }
            return It->second;
        }


        //called while parsing <nd ref="..."/> elements
        //marked override because streetmap defines as virtual
        void AddNodeID(TNodeID nodeid) override{
            DNodeIDs.push_back(nodeid);
        }

        //called while parsing <tag k= and v=> elements in a way
        void SetAttribute(const std::string &key, const std::string &value){
            if(DAttributes.find(key) == DAttributes.end()){
                DAttributeKeys.push_back(key);
            }
            DAttributes[key] = value;
        }
        
    };


    //storage containers
    std::vector<std::shared_ptr<SNode>> DNodesByIndex;
    std::unordered_map<TNodeID,std::shared_ptr<SNode>> DNodesByID;

    std::vector<std::shared_ptr<SWay>> DWaysByIndex;
    std::unordered_map<TWayID,std::shared_ptr<SWay>> DWaysByID;



    //finds the next occurence of a start tag withthe given name
    //return true if found and false if not and we reach the end of stream
    bool FindStartTag(std::shared_ptr< CXMLReader > xmlsource, const std::string &starttag){
        SXMLEntity TempEntity;
        while(xmlsource->ReadEntity(TempEntity,true)){
            if((TempEntity.DType == SXMLEntity::EType::StartElement)&&(TempEntity.DNameData == starttag)){
                return true;
            }
        }
        return false;
    }

    //finds next occurrence of and end tag with the given name
    bool FindEndTag(std::shared_ptr< CXMLReader > xmlsource, const std::string &starttag){
        SXMLEntity TempEntity;
        while(xmlsource->ReadEntity(TempEntity,true)){
            if((TempEntity.DType == SXMLEntity::EType::EndElement)&&(TempEntity.DNameData == starttag)){
                return true;
            }
        }
        return false;
    }


    // Parse a single <node> and tag descriptors
    //uses src as a CXMLReader stream
    //nodeentity is the start element entitiyu that contains the id/latitude/longitude

    //builds an SNode from the descriptor
    //reads the nested tag elemnet and stores as an attr
    //stop when it hits the ending node and adds the node to DNodesByIndex and DNodesByIndex
void ParseNode(const std::shared_ptr<CXMLReader> &src, const SXMLEntity &NodeEntity){
    auto NewNode = std::make_shared<SNode>(NodeEntity);

    SXMLEntity TempEntity;
    while(src->ReadEntity(TempEntity, true)){
        if(TempEntity.DType == SXMLEntity::EType::StartElement && TempEntity.DNameData == DTagTag){
            const std::string Key = TempEntity.AttributeValue(DTagKeyAttr);
            const std::string Val = TempEntity.AttributeValue(DTagValueAttr);
            if(!Key.empty()){
                NewNode->SetAttribute(Key, Val);
            }
        }
        else if(TempEntity.DType == SXMLEntity::EType::EndElement && TempEntity.DNameData == DNodeTag){
            break;
        }
    }

    DNodesByIndex.push_back(NewNode);
    DNodesByID[NewNode->ID()] = NewNode;
}

// Parse a single <way> including tag descriptors
//Builds an SWay from the wayid
//reads the <nd... and apprends the node id to the list
//stops when it hits the ending way and adds it to DWaysByIndex and DwaysbyID
void ParseWay(const std::shared_ptr<CXMLReader> &src, const SXMLEntity &WayEntity){
    auto NewWay = std::make_shared<SWay>(WayEntity);

    SXMLEntity TempEntity;
    while(src->ReadEntity(TempEntity, true)){
        if(TempEntity.DType == SXMLEntity::EType::StartElement && TempEntity.DNameData == DNDTag){
            const std::string RefStr = TempEntity.AttributeValue(DNDRefAttr);
            if(!RefStr.empty()){
                NewWay->AddNodeID(std::stoull(RefStr));
            }
        }
        else if(TempEntity.DType == SXMLEntity::EType::StartElement && TempEntity.DNameData == DTagTag){
            const std::string Key = TempEntity.AttributeValue(DTagKeyAttr);
            const std::string Val = TempEntity.AttributeValue(DTagValueAttr);
            if(!Key.empty()){
                NewWay->SetAttribute(Key, Val);
            }
        }
        else if(TempEntity.DType == SXMLEntity::EType::EndElement && TempEntity.DNameData == DWayTag){
            break;
        }
    }

    DWaysByIndex.push_back(NewWay);
    DWaysByID[NewWay->ID()] = NewWay;
}



    //parses the whole XML document
    //seeks the <osm> tag and loops reading entities: node, way and ending osm until we hit the ending osm to stop
    bool ParseOSM(std::shared_ptr<CXMLReader> src){
        if(!FindStartTag(src, DOSMTag)){
            return false;
        }

        SXMLEntity TempEntity;
        while(src->ReadEntity(TempEntity, true)){
            if(TempEntity.DType == SXMLEntity::EType::StartElement && TempEntity.DNameData == DNodeTag){
                ParseNode(src, TempEntity);
            }
            else if(TempEntity.DType == SXMLEntity::EType::StartElement && TempEntity.DNameData == DWayTag){
                ParseWay(src, TempEntity);
            }
            else if(TempEntity.DType == SXMLEntity::EType::EndElement && TempEntity.DNameData == DOSMTag){
                break; // done with </osm>
            }
        }

        return true;
    }

    SImplementation(std::shared_ptr<CXMLReader> src){
        ParseOSM(src);
    }

    std::size_t NodeCount() const noexcept{
        return DNodesByIndex.size();
    }

    std::size_t WayCount() const noexcept{
        return DWaysByIndex.size();
    }

    std::shared_ptr<CStreetMap::SNode> NodeByIndex(std::size_t index) const noexcept{
        if(index < DNodesByIndex.size()){
            return DNodesByIndex[index];
        }
        return nullptr;
    }

    std::shared_ptr<CStreetMap::SNode> NodeByID(TNodeID id) const noexcept{
        auto Search = DNodesByID.find(id);
        if(Search != DNodesByID.end()){
            return Search->second;
        }
        return nullptr;
    }

    std::shared_ptr<CStreetMap::SWay> WayByIndex(std::size_t index) const noexcept{
        if(index < DWaysByIndex.size()){
            return DWaysByIndex[index];
        }
        return nullptr;
    }

    std::shared_ptr<CStreetMap::SWay> WayByID(TWayID id) const noexcept{
        auto Search = DWaysByID.find(id);
        if(Search != DWaysByID.end()){
            return Search->second;
        }
        return nullptr;
    }

};


COpenStreetMap::COpenStreetMap(std::shared_ptr<CXMLReader> src){
    DImplementation = std::make_unique<SImplementation>(src);
}

COpenStreetMap::~COpenStreetMap(){
    
}

std::size_t COpenStreetMap::NodeCount() const noexcept{
    return DImplementation->NodeCount();
}

std::size_t COpenStreetMap::WayCount() const noexcept{
    return DImplementation->WayCount();
}

std::shared_ptr<CStreetMap::SNode> COpenStreetMap::NodeByIndex(std::size_t index) const noexcept{
    return DImplementation->NodeByIndex(index);
}

std::shared_ptr<CStreetMap::SNode> COpenStreetMap::NodeByID(TNodeID id) const noexcept{
    return DImplementation->NodeByID(id);
}

std::shared_ptr<CStreetMap::SWay> COpenStreetMap::WayByIndex(std::size_t index) const noexcept{
    return DImplementation->WayByIndex(index);
}

std::shared_ptr<CStreetMap::SWay> COpenStreetMap::WayByID(TWayID id) const noexcept{
    return DImplementation->WayByID(id);
}

