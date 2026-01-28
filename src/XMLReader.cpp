#include "XMLReader.h"
#include <expat.h>
#include <queue>

struct CXMLReader::SImplementation{


    static void ExpatStartElement(void *data, const XML_Char *name, const XML_Char **attrs){

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
