#include "SVGWriter.h"
#include "svg.h"
#include <string>
#include <vector>
#include <iostream>

struct CSVGWriter::SImplementation{
    
};

CSVGWriter::CSVGWriter(std::shared_ptr< CDataSink > sink, TSVGPixel width, TSVGPixel height){

}

CSVGWriter::~CSVGWriter(){

}

bool CSVGWriter::Circle(const SSVGPoint &center, TSVGReal radius, const TAttributes &style){

}

bool CSVGWriter::Rectange(const SSVGPoint &topleft, const SSVGSize &size, const TAttributes &style){

}

bool CSVGWriter::Line(const SSVGPoint &start, const SSVGPoint &end, const TAttributes &style){

}

bool CSVGWriter::SimplePath(const std::vector<SSVGPoint> points, const TAttributes &style){

}

bool CSVGWriter::GroupBegin(const TAttributes &attrs){

}

bool CSVGWriter::GroupEnd(){

}