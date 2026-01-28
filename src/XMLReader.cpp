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

       







    }

    static void ExpatEndElement(void *data, const XML_Char *name){

    }

    static void ExpatCharacterData(void *data, const XML_Char *s, int len){
        
    }

    SImplementation(std::shared_ptr< CDataSource > src){

    }

    ~SImplementation(){

    }

    bool End() const{

        //temp
        return true;

    }

    bool ReadEntity(SXMLEntity &entity, bool skipcdata){

        //temp
        return false;

    }
    

};

CXMLReader::CXMLReader(std::shared_ptr< CDataSource > src){

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
