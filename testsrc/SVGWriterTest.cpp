#include <gtest/gtest.h>
#include "SVGWriter.h"
#include "StringDataSink.h"

TEST(SVGWriterTest, CreateDestroyTest){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    {
        CSVGWriter Writer(Sink,100,50);
    }
    EXPECT_EQ(Sink->String(),"<?xml version=\"1.0\" encoding=\"UTF-8\"?><svg width=\"100\" height=\"50\" xmlns=\"http://www.w3.org/2000/svg\"></svg>");
}

TEST(SVGWriterTest, CircleTest){
    
}

TEST(SVGWriterTest, RectangleTest){

}

TEST(SVGWriterTest, LineTest){

}

TEST(SVGWriterTest, SimplePathTest){

}

TEST(SVGWriterTest, GroupTest){

}

class CFailingSink : public CDataSink{
    public:
        int DValidCalls = 0;
        virtual ~CFailingSink(){};
        bool Put(const char &ch) noexcept override{
            if(DValidCalls){
                DValidCalls--;
                return true;
            }
            return false;
        }

        bool Write(const std::vector<char> &buf) noexcept override{
            if(DValidCalls){
                DValidCalls--;
                return true;
            }
            return false;
        }
};

TEST(SVGWriterTest, ErrorTests){

}
