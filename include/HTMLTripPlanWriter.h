#ifndef HTMLTRIPWRITER_H
#define HTMLTRIPWRITER_H

#include "TripPlanWriter.h"
#include "StreetMap.h"
#include <string_view>

class CHTMLTripPlanWriter: public CTripPlanWriter{
    private:
        struct SImplementation;
        std::unique_ptr<SImplementation> DImplementation;

    public:
        //inline static constexpr std::string_view HTMLWidth = "svg-width";

        CHTMLTripPlanWriter(std::shared_ptr<CStreetMap> streetmap, std::shared_ptr<CBusSystem> bussystem);
        ~CHTMLTripPlanWriter();

        std::shared_ptr<SConfig> Config() const override;

        bool WritePlan(std::shared_ptr<CDataSink> sink, const TTravelPlan &plan) override;
};

#endif
