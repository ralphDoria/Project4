#include "XMLReader.h"
#include <expat.h>
#include <queue>

struct CXMLReader::SImplementation{

    std::shared_ptr<CDataSource> DSource; //do various actions(tell if done, give chars, read parts of XML), the input
    XML_Parser DParser = nullptr; // expat parser instance

    std::queue<SXMLEntity> DQueue;//when the parser reads a tag or text of XML, it wants to remember it to make sure it ends later, so it goes in the queue and gets checks later, and the items stack up for nested XML functions

    bool DDone = false; //bool to check for End()
    bool DFailed = false; //bool to check for error


    static void ExpatStartElement(void *data, const XML_Char *name, const XML_Char **attrs){
        /*
            we want this function to read the first starting tag
            it should set the type to StartElement from the EType enum
            make sure it knows the name of the type of tag
            add the correct attributes
            and add the tag to the queue to check for an ending later if needed
        */

        auto *Self = static_cast<SImplementation *>(data);

        SXMLEntity entity;

        //make it a start tag type
        entity.DType = SXMLEntity::EType::StartElement;
        //store the name
        entity.DNameData = name ? std::string(name) : std::string();

        //add attributes - the characters that change the XML output
        if(attrs){

            //iterate through the chars of the tag as a list that gets assigned to the attribute at that index
            for(std::size_t i = 0; attrs[i] != nullptr; i+=2){
                const char *AName = attrs[i];
                const char *AVal = attrs[i+1];

                if(AName && AVal){
                    entity.SetAttribute(AName, AVal);
                }
            }
            //add this entity to the queue to remember for the callback
            Self->DQueue.push(entity);

        }

    }

    static void ExpatEndElement(void *data, const XML_Char *name){
        //kijnd of like the same thing as startelement but no need for attrs and change everything to End type of element

        auto *Self = static_cast<SImplementation *>(data);

        SXMLEntity entity;

        entity.DType = SXMLEntity::EType::EndElement;

        entity.DNameData = name ? std::string(name) : std::string();

        Self->DQueue.push(entity);

    }

    static void ExpatCharacterData(void *data, const XML_Char *s, int len){
        /*
            label as CharacterData
            store text in NameData
            add into the queue
        */

        auto *Self = static_cast<SImplementation *>(data);

        //ignore empty characterdata
        if(!s || len <= 0){
            return ;
        }


        //otherwise create a new character data piece
        SXMLEntity entity;
        entity.DType = SXMLEntity::EType::CharData;
        entity.DNameData = std::string(s, static_cast<std::size_t>(len));

        
        Self->DQueue.push(entity);

    }

    SImplementation(std::shared_ptr< CDataSource > src)
        : DSource(std::move(src)) //member variable - Dsource is the src
    {
        //constructor must store the data source given, create a parser for expat, and make this object main to send functions to

        //creating a parser instance for this object
        DParser = XML_ParserCreate(nullptr);

        //using this object as teh data source ahd passes back a pointer to this object
        XML_SetUserData(DParser, this);

        //adding the start and end tags as a callback option
        XML_SetElementHandler(
            DParser,
            ExpatStartElement,
            ExpatEndElement
        );

        //add character data as a callback
        XML_SetCharacterDataHandler(
            DParser,
            ExpatCharacterData
        );
    }

    ~SImplementation(){
        //destroy anything from the object that is using memory or needs to be freed

        if(DParser){
            XML_ParserFree(DParser);
            DParser = nullptr;
        }

    }

    bool End() const{

        //it is at the end if there is no more XML to read or something failed, and the queue being empty should follow being true as well
        return(DDone || DFailed) && DQueue.empty();

    }

    void SendToExpat(){ // helper function reading bytes from the Data source and sending it to the expat parser to add to queue

        if(DDone || DFailed){
            return ;
        }

        //if its at the end of XML pieces, then its the last piece
        if(DSource->End()){

            if(XML_Parse(DParser, "", 0, XML_TRUE) == XML_STATUS_ERROR){
                DFailed = true;
            }else{
                DDone = true;
            }
            return ;
        }

        //Read a piece of XML bytes from the source
        std::vector<char> buffer;
        const std::size_t ChunkSize = 512;
        if(!DSource->Read(buffer, ChunkSize)){

            if(XML_Parse(DParser, "", 0, XML_TRUE) == XML_STATUS_ERROR){
                DFailed = true;
            }else{
                DDone = true;
            }
            return ;
        }

        const int finalPiece = DSource->End() ? XML_TRUE : XML_FALSE;
        if(XML_Parse(DParser, buffer.data(), static_cast<int>(buffer.size()), finalPiece) == XML_STATUS_ERROR){
            DFailed = true;
            return ;
        }

        if(finalPiece){
            DDone = true;
        }
    }

    bool ReadEntity(SXMLEntity &entity, bool skipcdata){

        (void)skipcdata;

        while(DQueue.empty() && !(DDone || DFailed)){
            SendToExpat();
        }

        if(DQueue.empty()){
            return false;
        }

        if(skipcdata){
            while(true){
                while(DQueue.empty() && !(DDone || DFailed)){
                    SendToExpat();
                }
                if(DQueue.empty()){
                    return false;
                }

                if(DQueue.front().DType != SXMLEntity::EType::CharData){
                    break;
                }
                DQueue.pop();
            }
        }


        if(DQueue.front().DType == SXMLEntity::EType::CharData){
            SXMLEntity merged = DQueue.front();
            DQueue.pop();

            while(true){

                while(DQueue.empty() && !(DDone || DFailed)){
                    SendToExpat();
                }
                if(DQueue.empty()){
                    return false;
                }

                if(DQueue.front().DType != SXMLEntity::EType::CharData){
                    break;
                }
                merged.DNameData += DQueue.front().DNameData;
                DQueue.pop();
            }
            entity = merged;
            return true;

        }


        entity = DQueue.front();
        DQueue.pop();
        return true;

    }
    

};

CXMLReader::CXMLReader(std::shared_ptr< CDataSource > src)
    : DImplementation(std::make_unique<SImplementation>(std::move(src)))
{


}

CXMLReader::~CXMLReader() = default; //temp default


/*
    checks if we're done reading the entire XML
    true if there are no more tags to read and we're at the end of the source
    false if there are still tags to read
*/
bool CXMLReader::End() const{

    //temp
    return DImplementation->End();

}

/*
    takes in the next tag and fills the next tag in with whatever was found
    true if there was a tag to be read
    false if we are at the end
    @skipcdata is if we should ignore whatever chars are being read if its not a tag
*/
bool CXMLReader::ReadEntity(SXMLEntity &entity, bool skipcdata){
    
    //temp
    return DImplementation->ReadEntity(entity, skipcdata);
}
