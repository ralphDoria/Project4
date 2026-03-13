#include <gtest/gtest.h>
#include "SVGWriter.h"
#include "StringDataSink.h"

TEST(SVGWriterTest, CreateDestroyTest){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    {
        CSVGWriter Writer(Sink,100,50);
    }
    EXPECT_EQ(Sink->String(),"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<svg width=\"100\" height=\"50\" xmlns=\"http://www.w3.org/2000/svg\">\n"
"</svg>\n");

}

TEST(SVGWriterTest, CircleTest){
    
    //sink captures all the stuff written by CSVGWriter into one string
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();

    //needed to create a scope so the destructor can run before check
    {
        CSVGWriter Writer(Sink, 100, 50);

        //defining circle specs
        SSVGPoint Center{50.0, 25.0};
        TSVGReal Radius = 10.0;

        //style attrs
        TAttributes Style;
        Style.push_back({"fill", "none"});
        Style.push_back({"stroke", "green"});

        //return true if svgcircle reutnrs svg_ok
        bool test = Writer.Circle(Center, Radius, Style);
        EXPECT_TRUE(test);
    }

    //check for output of the final string
    EXPECT_NE(Sink->String().find("<circle"), std::string::npos);

}

TEST(SVGWriterTest, RectangleTest){

    
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();

    {
        CSVGWriter Writer(Sink, 100, 50);

        SSVGPoint TopLeft{10.0, 5.0};
        SSVGSize Size{30.0, 20.0};

        TAttributes Style;
        Style.push_back({"fill","none"});
        Style.push_back({"stroke","green"});

        bool test = Writer.Rectange(TopLeft, Size, Style);
        EXPECT_TRUE(test);
    }

        std::string Expected = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<svg width=\"100\" height=\"50\" xmlns=\"http://www.w3.org/2000/svg\">\n"
            "  <rect x=\"10.000000\" y=\"5.000000\" width=\"30.000000\" height=\"20.000000\" style=\"fill:none; stroke:green\"/>\n"
            "</svg>\n";
    
        
    EXPECT_EQ(Sink->String(), Expected);
}


TEST(SVGWriterTest, LineTest){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    {
        CSVGWriter Writer(Sink, 100, 100);

        SSVGPoint Start{10.0, 10.0};
        SSVGPoint End{90.0, 90.0};

        TAttributes Style;
        Style.push_back({"stroke","green"});

        bool test = Writer.Line(Start, End, Style);
        EXPECT_TRUE(test);

    }

    std::string Expected =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<svg width=\"100\" height=\"100\" xmlns=\"http://www.w3.org/2000/svg\">\n"
        "  <line x1=\"10.000000\" y1=\"10.000000\" x2=\"90.000000\" y2=\"90.000000\" style=\"stroke:green\"/>\n"
        "</svg>\n";

    EXPECT_EQ(Sink->String(), Expected);
}

TEST(SVGWriterTest, SimplePathTest){

    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();

    {
        CSVGWriter Writer(Sink, 100, 100);

        //simple path is an SVG path tghat connects points
        //M for the first points and L to allign
        std::vector<SSVGPoint> Points = {{10.0, 10.0}, {50.0, 50.0}, {90.0, 10.0}};

        TAttributes Style;
        Style.push_back({"stroke","black"});

        bool test = Writer.SimplePath(Points, Style);
        EXPECT_TRUE(test);
    }

    std::string Expected =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<svg width=\"100\" height=\"100\" xmlns=\"http://www.w3.org/2000/svg\">\n"
        "  <path d=\"M 10.000000 10.000000 L 50.000000 50.000000 L 90.000000 10.000000\" style=\"stroke:black\"/>\n"
        "</svg>\n";

    EXPECT_EQ(Sink->String(), Expected);
}

TEST(SVGWriterTest, GroupTest){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();

    {
        CSVGWriter Writer(Sink, 100, 100);

        
        TAttributes GroupAttrs;
        GroupAttrs.push_back({"stroke", "blue"});

        EXPECT_TRUE(Writer.GroupBegin(GroupAttrs));

        SSVGPoint Center{50.0, 50.0};
        TAttributes Style;
        Style.push_back({"fill", "none"});
        EXPECT_TRUE(Writer.Circle(Center, 10.0, Style));

        EXPECT_TRUE(Writer.GroupEnd());
    }

    std::string Expected =  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<svg width=\"100\" height=\"100\" xmlns=\"http://www.w3.org/2000/svg\">\n"
        "  <g stroke:blue>\n"
        "  <circle cx=\"50.000000\" cy=\"50.000000\" r=\"10.000000\" style=\"fill:none\"/>\n"
        "  </g>\n"
        "</svg>\n";

    EXPECT_EQ(Sink->String(), Expected);
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

    //only allows 1 fail so that the SVG can return false
        std::shared_ptr<CFailingSink> Sink = std::make_shared<CFailingSink>();


        Sink->DValidCalls = 1;

        CSVGWriter Writer(Sink, 100, 100);

        SSVGPoint Center{50.0, 50.0};
        TAttributes Style;

        EXPECT_FALSE(Writer.Circle(Center, 10.0, Style));
}
