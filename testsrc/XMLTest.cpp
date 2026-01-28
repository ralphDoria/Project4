#include <gtest/gtest.h>
#include "XMLReader.h"
#include "StringDataSource.h"

TEST(XMLReaderTest, SimpleTest){
    
    //test XML input
    const std::string xml = "<a/>";

    // creating a data source object so the reader can read the XML
    std::shared_ptr<CDataSource> src = std::make_shared<CStringDataSource>(xml);

    //constructing the XML reader using the data source
    CXMLReader reader(src);

    //reads the next XML entitiy created by the reader
    SXMLEntity entity;
    bool ok = reader.ReadEntity(entity);

    ASSERT_TRUE(ok);

    //expecting a Completeelement named "a" from the test XML
    EXPECT_EQ(SXMLEntity::EType::CompleteElement, entity.DType);
    EXPECT_EQ("a", entity.DNameData);

    //no attribute w the a tag
    EXPECT_TRUE(entity.DAttributes.empty());

    //after reading the entitiy the reader should be at the end
    EXPECT_TRUE(reader.End());
    
}

TEST(XMLReaderTest, ElementTest){

}

TEST(XMLReaderTest, CDataTest){

}

TEST(XMLReaderTest, LongCDataTest){

}

TEST(XMLReaderTest, SpecialCharacterTest){

}

TEST(XMLReaderTest, InvalidXMLTest){

}

TEST(XMLReaderTest, LongCharDataCrosses512Boundary){

}
