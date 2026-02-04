#include "XMLReader.h"
#include <expat.h>
#include <queue>

struct CXMLReader::SImplementation{
    
};

CXMLReader::CXMLReader(std::shared_ptr< CDataSource > src){

}

CXMLReader::~CXMLReader(){

}

bool CXMLReader::End() const{

}

bool CXMLReader::ReadEntity(SXMLEntity &entity, bool skipcdata){

}
