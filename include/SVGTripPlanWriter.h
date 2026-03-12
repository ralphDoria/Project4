#ifndef SVGTRIPWRITER_H
#define SVGTRIPWRITER_H

#include "TripPlanWriter.h"
#include "StreetMap.h"
#include <string_view>

class CSVGTripPlanWriter: public CTripPlanWriter{
    private:
        struct SImplementation;
        std::unique_ptr<SImplementation> DImplementation;

    public:
        inline static constexpr std::string_view SVGWidth = "svg-width";
        inline static constexpr std::string_view SVGHeight = "svg-height";
        inline static constexpr std::string_view SVGMarginPixels = "svg-margin";
        inline static constexpr std::string_view SourceColor = "source-color";
        inline static constexpr std::string_view DestinationColor = "destination-color";
        inline static constexpr std::string_view BusStopRadius = "busstop-radius";
        inline static constexpr std::string_view SourceRadius = "source-radius";
        inline static constexpr std::string_view DestinationRadius = "destination-radius";
        inline static constexpr std::string_view BusColor0 = "bus-color-0";
        inline static constexpr std::string_view BusColor1 = "bus-color-1";
        inline static constexpr std::string_view StreetColor = "street-color";
        inline static constexpr std::string_view MotorwayStroke = "motorway-stroke";
        inline static constexpr std::string_view PrimaryStroke = "primary-stroke";
        inline static constexpr std::string_view SecondaryStroke = "secondary-stroke";
        inline static constexpr std::string_view TertiaryStroke = "tertiary-stroke";
        inline static constexpr std::string_view ResidentialStroke = "residential-stroke";
        inline static constexpr std::string_view BusStroke = "bus-stroke";
        inline static constexpr std::string_view LabelMargin = "label-margin";
        inline static constexpr std::string_view LabelColor = "label-color";
        inline static constexpr std::string_view LabelBackground = "label-background";
        inline static constexpr std::string_view LabelSize = "label-size";
        inline static constexpr std::string_view LabelPaintOrder = "label-paint-order";
        inline static constexpr std::string_view MotorwayEnabled = "motorway-enabled";
        inline static constexpr std::string_view PrimaryEnabled = "primary-enabled";
        inline static constexpr std::string_view SecondaryEnabled = "secondary-enabled";
        inline static constexpr std::string_view TertiaryEnabled = "tertiary-enabled";
        inline static constexpr std::string_view ResidentialEnabled = "residential-enabled";

        CSVGTripPlanWriter(std::shared_ptr<CStreetMap> streetmap, std::shared_ptr<CBusSystem> bussystem);
        ~CSVGTripPlanWriter();

        std::shared_ptr<SConfig> Config() const override;

        bool WritePlan(std::shared_ptr<CDataSink> sink, const TTravelPlan &plan) override;
};

#endif
