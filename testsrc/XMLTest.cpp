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
    ASSERT_TRUE(reader.ReadEntity(entity));

    //expecting the entitiy to be of a CompleteElement type from the enum EType
    EXPECT_EQ(SXMLEntity::EType::StartElement, entity.DType);
    EXPECT_EQ("a", entity.DNameData);


    //need to do it for the closing tag
    SXMLEntity entity2;
    ASSERT_TRUE(reader.ReadEntity(entity2));

    EXPECT_EQ(SXMLEntity::EType::EndElement, entity2.DType);
    EXPECT_EQ("a", entity2.DNameData);

    //after reading all XML entities there should be no more XML stuff
    EXPECT_TRUE(reader.End());
    
}

TEST(XMLReaderTest, ElementTest){

    const std::string xml = "<a></a>";
    std::shared_ptr<CDataSource> src = std::make_shared<CStringDataSource>(xml);
    CXMLReader reader(src);

    //need to create two entities and make sure they both exist

    SXMLEntity e1;
    bool ok1 = reader.ReadEntity(e1);
    ASSERT_TRUE(ok1);
    EXPECT_EQ(SXMLEntity::EType::StartElement, e1.DType);
    EXPECT_EQ("a", e1.DNameData);
    EXPECT_TRUE(e1.DAttributes.empty());

    //second entitiy
    SXMLEntity e2;
    bool ok2 = reader.ReadEntity(e2);
    ASSERT_TRUE(ok2);
    EXPECT_EQ(SXMLEntity::EType::EndElement, e2.DType);
    EXPECT_EQ("a", e2.DNameData);
    EXPECT_TRUE(e2.DAttributes.empty());

    EXPECT_TRUE(reader.End());

}

TEST(XMLReaderTest, CDataTest){

    const std::string xml = "<a>test</a>";
    std::shared_ptr<CDataSource> src = std::make_shared<CStringDataSource>(xml);
    CXMLReader reader(src);

    //create three entities, open tag, char text, end tag
    SXMLEntity e1;
    bool ok1 = reader.ReadEntity(e1);
    ASSERT_TRUE(ok1);
    EXPECT_EQ(SXMLEntity::EType::StartElement, e1.DType);
    EXPECT_EQ("a", e1.DNameData);
    EXPECT_TRUE(e1.DAttributes.empty());

    SXMLEntity e2;
    bool ok2 = reader.ReadEntity(e2);
    ASSERT_TRUE(ok2);
    EXPECT_EQ(SXMLEntity::EType::CharData, e2.DType);
    EXPECT_EQ("test", e2.DNameData);
    EXPECT_TRUE(e2.DAttributes.empty());

    SXMLEntity e3;
    bool ok3 = reader.ReadEntity(e3);
    ASSERT_TRUE(ok3);
    EXPECT_EQ(SXMLEntity::EType::EndElement, e3.DType);
    EXPECT_EQ("a", e3.DNameData);
    EXPECT_TRUE(e3.DAttributes.empty());

    EXPECT_TRUE(reader.End());

}

TEST(XMLReaderTest, LongCDataTest){

    const std::string longText(1000, 'x'); //x 1k times
    const std::string xml = "<a>" + longText + "</a>";

    std::shared_ptr<CDataSource> src = std::make_shared<CStringDataSource>(xml);
    CXMLReader reader(src);

    //same thing as before, read the opening, long data text, end
    SXMLEntity e1;
    ASSERT_TRUE(reader.ReadEntity(e1));
    EXPECT_EQ(SXMLEntity::EType::StartElement, e1.DType);
    EXPECT_EQ("a", e1.DNameData);

    SXMLEntity e2;
    ASSERT_TRUE(reader.ReadEntity(e2));
    EXPECT_EQ(SXMLEntity::EType::CharData, e2.DType);
    EXPECT_EQ(longText, e2.DNameData);

    SXMLEntity e3;
    ASSERT_TRUE(reader.ReadEntity(e3));
    EXPECT_EQ(SXMLEntity::EType::EndElement, e3.DType);
    EXPECT_EQ("a", e3.DNameData);

    EXPECT_TRUE(reader.End());

}

TEST(XMLReaderTest, SpecialCharacterTest){
    //make sure if an XML characterl ike <> or something like that is typed in, we are able to escape it

    const std::string xml = "<a>&lt; &amp; &gt;</a>";
    std::shared_ptr<CDataSource> src = std::make_shared<CStringDataSource>(xml);
    CXMLReader reader(src);

    //same as before, just decode the special char properly

    SXMLEntity e1;
    ASSERT_TRUE(reader.ReadEntity(e1));
    EXPECT_EQ(SXMLEntity::EType::StartElement, e1.DType);
    EXPECT_EQ("a", e1.DNameData);

    SXMLEntity e2;
    ASSERT_TRUE(reader.ReadEntity(e2));
    EXPECT_EQ(SXMLEntity::EType::CharData, e2.DType);
    EXPECT_EQ("< & >", e2.DNameData);

    SXMLEntity e3;
    ASSERT_TRUE(reader.ReadEntity(e3));
    EXPECT_EQ(SXMLEntity::EType::EndElement, e3.DType);
    EXPECT_EQ("a", e3.DNameData);

    EXPECT_TRUE(reader.End());

}

TEST(XMLReaderTest, InvalidXMLTest){ //doesnt follow typical XML format, should error

    const std::string xml = "<a></b>";
    std::shared_ptr<CDataSource> src = std::make_shared<CStringDataSource>(xml);
    CXMLReader reader(src);

    SXMLEntity e1;
    bool first = reader.ReadEntity(e1);
    ASSERT_TRUE(first);
    EXPECT_EQ(SXMLEntity::EType::StartElement, e1.DType);
    EXPECT_EQ("a", e1.DNameData);

    //read until there is none left, we dont want to find a good closing tag, want it to error out
    bool secondTest = true;
    SXMLEntity temp;

    while(secondTest == true){
        secondTest = reader.ReadEntity(temp);
    }
    EXPECT_FALSE(secondTest);

    EXPECT_TRUE(reader.End());
}

TEST(XMLReaderTest, LongCharDataCrosses512Boundary){ //crosses the 512 bytes limit for the datasrouce

    const std::string longText(1000, 'J'); //text block over 512 bytes
    const std::string xml = "<a>" + longText + "</a>";

    std::shared_ptr<CDataSource> src = std::make_shared<CStringDataSource>(xml);
    CXMLReader reader(src);

    SXMLEntity e1;
    ASSERT_TRUE(reader.ReadEntity(e1));
    EXPECT_EQ(SXMLEntity::EType::StartElement, e1.DType);
    EXPECT_EQ("a", e1.DNameData);

    SXMLEntity e2;
    ASSERT_TRUE(reader.ReadEntity(e2));
    EXPECT_EQ(SXMLEntity::EType::CharData, e2.DType);
    EXPECT_EQ(longText, e2.DNameData);

    SXMLEntity e3;
    ASSERT_TRUE(reader.ReadEntity(e3));
    EXPECT_EQ(SXMLEntity::EType::EndElement, e3.DType);
    EXPECT_EQ("a", e3.DNameData);

    EXPECT_TRUE(reader.End());

}
