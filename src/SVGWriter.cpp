#include "SVGWriter.h"
#include "svg.h"
#include <string>
#include <vector>
#include <iostream>

/*
    CSVGWriter is a wrapper for C++ around the svg.c
    holds unique ptr to SImplementation
*/

using std::cout;
using std::endl;

struct CSVGWriter::SImplementation{
    std::shared_ptr< CDataSink > DSink;
    svg_context_ptr DContext;


    //svg_create usies WriteFunction to write text and gets sent to CDataSink
    static svg_return_t WriteFunction(svg_user_context_ptr user, const char *text){
        SImplementation *Implementation = (SImplementation *)user;

        while(*text){
            if(!Implementation->DSink->Put(*text)){
                return SVG_ERR_IO;
            }
            text++;
        }
        return SVG_OK;   
    }

    static svg_return_t CleanupFunc(svg_user_context_ptr user){
        (void)user;
        return SVG_OK;
    }


    SImplementation(std::shared_ptr< CDataSink > sink, TSVGPixel width, TSVGPixel height){
        DSink = sink;
        DContext = svg_create(WriteFunction,CleanupFunc,this,width,height);
    }

    ~SImplementation(){
        svg_destroy(DContext);
    }

    std::string CreateStyleString(const TAttributes &style){
        std::string styleString;

        for(std::size_t i = 0; i < style.size(); i++){

            styleString+= style[i].first;
            styleString += ":";
            styleString += style[i].second;

            if(i+1 < style.size()){
                styleString += "; ";

            }

        }

        return styleString;
    }

    //draws circle by calling svg_circle()
    bool Circle(const SSVGPoint &center, TSVGReal radius, const TAttributes &style){
        //converst teh C++ SSVGPoint into C svg_point_t
        svg_point_t Center{center.DX,center.DY};
        std::string Style = CreateStyleString(style);
        
        svg_return_t Result = svg_circle(DContext, &Center, radius, Style.empty() ? nullptr : Style.c_str());
        
        return Result == SVG_OK;
    }

    bool Rectange(const SSVGPoint &topleft, const SSVGSize &size, const TAttributes &style){

        //creating objects of the c struct
        svg_point_t TopLeft{topleft.DX, topleft.DY};
        svg_size_t Size{size.DWidth, size.DHeight};

        //creates the style in text
        std::string Style = CreateStyleString(style);

        //if the style is empty, pass nullptr, if it has something then you pass that
        svg_return_t Result = svg_rect(DContext, &TopLeft, &Size, Style.empty() ? nullptr : Style.c_str());

        //true only if svg_circle returns svg_ok
        return Result == SVG_OK;


    }

    bool Line(const SSVGPoint &start, const SSVGPoint &end, const TAttributes &style){

        svg_point_t Start{start.DX, start.DY};
        svg_point_t End{end.DX, end.DY};

        std::string Style = CreateStyleString(style);

        svg_return_t Result = svg_line(DContext, &Start, &End, Style.empty() ? nullptr : Style.c_str());
        
        return Result == SVG_OK;


    }

    bool SimplePath(const std::vector<SSVGPoint> points, const TAttributes &style){

        if(points.empty()){
            return false;
        }

        //build the attr string using path commands
        std::string Path = "M ";
        
        char buffer[64];

        //first points M * y
        snprintf(buffer, sizeof(buffer), "%f %f", points[0].DX, points[0].DY);
        Path+=buffer;

        //the rest of the points
        for(std::size_t i = 1; i < points.size(); i++){
            Path+= " L ";
            snprintf(buffer, sizeof(buffer), "%f %f", points[i].DX, points[i].DY);
            Path+=buffer;
        }

        std::string Style = CreateStyleString(style);

        //constructs the rest of the SVG element

        std::string Output = "  <path d=\"";
        Output += Path;
        Output += "\"";

        if(!Style.empty()){
            Output += " style=\"";
            Output += Style;
            Output += "\"";
        }

        Output += "/>\n";

        for(char c : Output){
            if(!DSink->Put(c)){
                return false;
            }
        }

        return true;

    }

    bool GroupBegin(const TAttributes &attrs){

        std::string AttrString = CreateStyleString(attrs);

        svg_return_t Result = svg_group_begin(DContext, AttrString.empty() ? nullptr : AttrString.c_str());

        return Result == SVG_OK;


    }

    bool GroupEnd(){

        svg_return_t Result = svg_group_end(DContext);
        return Result == SVG_OK;


    }

};

CSVGWriter::CSVGWriter(std::shared_ptr< CDataSink > sink, TSVGPixel width, TSVGPixel height){
    DImplementation = std::make_unique<SImplementation>(sink,width,height);
}

CSVGWriter::~CSVGWriter() = default;

bool CSVGWriter::Circle(const SSVGPoint &center, TSVGReal radius, const TAttributes &style){
    return DImplementation->Circle(center, radius,style);
}

bool CSVGWriter::Rectange(const SSVGPoint &topleft, const SSVGSize &size, const TAttributes &style){
    return DImplementation->Rectange(topleft,size,style);
}

bool CSVGWriter::Line(const SSVGPoint &start, const SSVGPoint &end, const TAttributes &style){
    return DImplementation->Line(start, end, style);    
}

bool CSVGWriter::SimplePath(const std::vector<SSVGPoint> points, const TAttributes &style){
    return DImplementation->SimplePath(points, style);  
}

bool CSVGWriter::GroupBegin(const TAttributes &attrs){
    return DImplementation->GroupBegin(attrs);  
}

bool CSVGWriter::GroupEnd(){
    return DImplementation->GroupEnd();  
}