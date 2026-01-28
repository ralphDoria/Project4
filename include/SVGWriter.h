#ifndef SVGWRITER_H
#define SVGWRITER_H

#include <memory>
#include <vector>
#include "DataSink.h"
#include "XMLEntity.h"

typedef int TSVGPixel;
typedef double TSVGReal;
typedef TSVGReal TSVGCoordinate;

struct SSVGPoint{
    TSVGCoordinate DX;
    TSVGCoordinate DY;
};

struct SSVGSize{
    TSVGReal DWidth;
    TSVGReal DHeight;
};

class CSVGWriter{
    private:
        struct SImplementation;
        std::unique_ptr<SImplementation> DImplementation;
        
    public:
        CSVGWriter(std::shared_ptr< CDataSink > sink, TSVGPixel width, TSVGPixel height);
        ~CSVGWriter();
        
        bool Circle(const SSVGPoint &center, TSVGReal radius, const TAttributes &style);
        bool Rectange(const SSVGPoint &topleft, const SSVGSize &size, const TAttributes &style);
        bool Line(const SSVGPoint &start, const SSVGPoint &end, const TAttributes &style);
        bool SimplePath(const std::vector<SSVGPoint> points, const TAttributes &style);
        bool GroupBegin(const TAttributes &attrs);
        bool GroupEnd();

};

#endif
                                      