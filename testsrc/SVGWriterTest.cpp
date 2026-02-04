#include <gtest/gtest.h>
#include "SVGWriter.h"
#include "StringDataSink.h"

TEST(SVGWriterTest, CreateDestroyTest){

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
