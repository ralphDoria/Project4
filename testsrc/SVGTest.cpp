#include "svg.h"
#include <gtest/gtest.h>
#include <string>
#include <vector>

// Helper structure to capture written SVG strings
struct STestOutput{
    std::vector<std::string> DLines;
    bool DDestroyed = false;

    std::string JoinOutput(){
        std::string Result;

        for (const auto& line : DLines){
            Result += line;
        }
        return Result;
    }
};

// Callback to capture SVG output
svg_return_t write_callback(svg_user_context_ptr user, const char* text){
    if(!user || !text){
        return SVG_ERR_NULL;
    }
    STestOutput* OutPtr = static_cast<STestOutput*>(user);
    OutPtr->DLines.push_back(text);
    return SVG_OK;
}

// Cleanup callback (just returns OK for testing)
svg_return_t cleanup_callback(svg_user_context_ptr user){
    if(!user){
        return SVG_ERR_NULL;
    }
    STestOutput* OutPtr = static_cast<STestOutput*>(user);
    if(OutPtr->DDestroyed){
        return SVG_ERR_STATE;
    }
    OutPtr->DDestroyed = true;
    return SVG_OK;
}

// --- TEST FIXTURE ---
class SVGTest : public ::testing::Test{
    protected:
        STestOutput DOutput;
        svg_context_ptr DContext = nullptr;

        void SetUp() override{
            DContext = svg_create(write_callback, cleanup_callback, &DOutput, 100, 100);
            ASSERT_NE(DContext, nullptr);
        }

        void TearDown() override{
            if(DContext){
                svg_destroy(DContext);
                DContext = nullptr;
            }
        }
};

// --- BASIC CREATION TEST ---
TEST_F(SVGTest, CreateAndDestroy){
    svg_context_ptr context = svg_create(write_callback,
                                         cleanup_callback,
                                         &DOutput,
                                         100,
                                         100);
    EXPECT_NE(context,nullptr);
    EXPECT_EQ(svg_destroy(context),SVG_OK);
}

// --- INVALID INPUT TESTS ---
TEST_F(SVGTest, NullContextFunctions){
    
    //creates point structs because the functions require them
    svg_point_t p{10, 10};
    svg_point_t q{20, 20};
    svg_size_t s{30, 40};

    //calling each of the functions and expecting a NULL enum value instead of an actual one
    //expecting the function call to return a NULL and it matches with the SVG NULL error
    EXPECT_EQ(svg_circle(NULL, &p, 5, "fill:black;"), SVG_ERR_NULL); //SVGERRNULL means a null pointer was passed to simulate a mistake in the code
    EXPECT_EQ(svg_rect(NULL, &p, &s, "fill:black;"), SVG_ERR_NULL);
    EXPECT_EQ(svg_line(NULL, &p, &q, "stroke:black;"), SVG_ERR_NULL);
    EXPECT_EQ(svg_group_begin(NULL, "id=\"g1\""), SVG_ERR_NULL);
    EXPECT_EQ(svg_group_end(NULL), SVG_ERR_NULL);

}

// --- DRAWING TESTS ---
//RUNNING A TEST THATS NAMED CIRCLE

using std::string;

TEST_F(SVGTest, Circle){
    DOutput.DLines.clear(); 
    //DOutput is an object created by the SVG Test setup and stores all the svg text
    //DLines is a SVG line that becomes a string
    //clear erases previous text because we are only checking circle

    svg_point_t c{50, 50}; //center point
    svg_return_t r = svg_circle(DContext, &c, 45, "fill:none; stroke:green;");//formats the string to create a circle

    EXPECT_EQ(r, SVG_OK); // ensuring that the circle function call succeeds

    ASSERT_EQ(DOutput.DLines.size(), 1u); //ensuring that the amount of lines written is 1

    std::string line = DOutput.DLines[0]; //takes the first line and stores it into line string variable

    EXPECT_NE(line.find("<circle"), string::npos);//searches the string to make sure that the <circle text appears
    EXPECT_NE(line.find("cx="), string::npos);
    EXPECT_NE(line.find("cy="), string::npos);
    EXPECT_NE(line.find("r="), string::npos);

    EXPECT_NE(line.find("stroke:green;"), string::npos);

    
}

TEST_F(SVGTest, Rectangle){
    DOutput.DLines.clear();

    svg_point_t left_top{10,20};
    svg_size_t size{30,40};

    svg_return_t r = svg_rect(DContext, &left_top, &size, "fill:black;");

    EXPECT_EQ(r, SVG_OK);

    ASSERT_EQ(DOutput.DLines.size(), 1u);

    std::string line = DOutput.DLines[0];

    EXPECT_NE(line.find("<rect"), string::npos);
    EXPECT_NE(line.find("x="), string::npos);
    EXPECT_NE(line.find("y="), string::npos);
    EXPECT_NE(line.find("width="), string::npos);
    EXPECT_NE(line.find("height="), string::npos);

    EXPECT_NE(line.find("fill:black;"), string::npos);

    
}

TEST_F(SVGTest, Line){
    DOutput.DLines.clear();
    svg_point_t start{10, 20};
    svg_point_t end{30, 40};

    svg_return_t r = svg_line(DContext, &start, &end, "stroke:black;");

    EXPECT_EQ(r, SVG_OK);

    ASSERT_EQ(DOutput.DLines.size(), 1u);

    std::string line = DOutput.DLines[0];

    EXPECT_NE(line.find("<line"), string::npos);
    EXPECT_NE(line.find("x1="), string::npos);
    EXPECT_NE(line.find("y1="), string::npos);
    EXPECT_NE(line.find("x2="), string::npos);
    EXPECT_NE(line.find("y2="), string::npos);

    EXPECT_NE(line.find("stroke:black;"), string::npos);
    
}

// --- GROUPING TEST ---
TEST_F(SVGTest, Grouping){
    DOutput.DLines.clear();

    svg_return_t r1 = svg_group_begin(DContext, "id=\"g1\"");
    EXPECT_EQ(r1, SVG_OK);

    svg_return_t r2 = svg_group_end(DContext);
    EXPECT_EQ(r2, SVG_OK);

    ASSERT_EQ(DOutput.DLines.size(), 2u);

    std::string open = DOutput.DLines[0];
    std::string close = DOutput.DLines[1];

    EXPECT_NE(open.find("<g"), string::npos);
    EXPECT_NE(open.find("id=\"g1\""), string::npos);

    EXPECT_NE(close.find("</g>"), string::npos);

    EXPECT_EQ(svg_group_end(DContext), SVG_ERR_STATE);
    
}

// --- EDGE CASES ---
TEST_F(SVGTest, ZeroDimensions){
    DOutput.DLines.clear();

    svg_point_t c{0, 0};
    EXPECT_EQ(svg_circle(DContext, &c, 0, NULL), SVG_OK);

    svg_point_t p{10, 10};
    svg_size_t  s{0, 0};
    EXPECT_EQ(svg_rect(DContext, &p, &s, NULL), SVG_OK);

    svg_point_t a{5, 5};
    svg_point_t b{5, 5};
    EXPECT_EQ(svg_line(DContext, &a, &b, NULL), SVG_OK);

    ASSERT_EQ(DOutput.DLines.size(), 3u);

    EXPECT_NE(DOutput.DLines[0].find("<circle"), std::string::npos);
    EXPECT_NE(DOutput.DLines[1].find("<rect"),   std::string::npos);
    EXPECT_NE(DOutput.DLines[2].find("<line"),   std::string::npos);
}


TEST_F(SVGTest, NullPointPointer){


    svg_point_t p{10,10};
    svg_point_t q{20,20};
    svg_size_t s{30,40};

    EXPECT_EQ(svg_circle(DContext, NULL, 5, "fill:black;"), SVG_ERR_NULL);
    
    EXPECT_EQ(svg_rect(DContext, NULL, &s, "fill:black;"), SVG_ERR_NULL);
    EXPECT_EQ(svg_rect(DContext, &p, NULL, "fill:black;"), SVG_ERR_NULL);

    EXPECT_EQ(svg_line(DContext, NULL, &q, "stroke:black;"), SVG_ERR_NULL);
    EXPECT_EQ(svg_line(DContext, &p, NULL, "stroke:black;"), SVG_ERR_NULL);

    
}

TEST_F(SVGTest, CreateEdgeCases){
    STestOutput out;

    EXPECT_EQ(svg_create(NULL, cleanup_callback, &out, 100, 100), nullptr);
    EXPECT_EQ(svg_create(write_callback, NULL, &out, 100, 100), nullptr); 

    EXPECT_EQ(svg_create(write_callback, cleanup_callback, &out, 0, 100), nullptr);
    EXPECT_EQ(svg_create(write_callback, cleanup_callback, &out, 100, 0), nullptr);
    EXPECT_EQ(svg_create(write_callback, cleanup_callback, &out, -1, 100), nullptr);
    EXPECT_EQ(svg_create(write_callback, cleanup_callback, &out, 100, -1), nullptr);

    EXPECT_EQ(svg_create(write_callback, cleanup_callback, NULL, 100, 100), nullptr);

}


TEST_F(SVGTest, DestroyEdgeCases){

    EXPECT_EQ(svg_destroy(NULL), SVG_ERR_NULL);


}

// Callback to capture SVG output
svg_return_t write_error_callback(svg_user_context_ptr user, const char* text){
    int *FailureCount = (int *)user;
    if(*FailureCount){
        (*FailureCount)--;
        return SVG_OK;    
    }
    return SVG_ERR_IO;
}

// --- IO Errors ---
TEST_F(SVGTest, IOErrorTest){

    int num_writes = 2;

    svg_context_ptr ctx = svg_create(write_error_callback, cleanup_callback, &num_writes, 100, 100);

    ASSERT_NE(ctx, nullptr);

    svg_point_t c{10,10};

    EXPECT_EQ(svg_circle(ctx, &c, 5, "fill:black;"), SVG_ERR_IO);

    EXPECT_EQ(svg_destroy(ctx), SVG_ERR_IO);


}