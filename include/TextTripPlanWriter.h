#ifndef TEXTTRIPWRITER_H
#define TEXTTRIPWRITER_H

#include "TripPlanWriter.h"

class CTextTripPlanWriter: public CTripPlanWriter{
    private:
        struct SImplementation;
        std::unique_ptr<SImplementation> DImplementation;

    public:
        inline static constexpr std::string_view Verbose = "verbose";

        CTextTripPlanWriter(std::shared_ptr<CBusSystem> bussystem);
        ~CTextTripPlanWriter();

        std::shared_ptr<SConfig> Config() const override;

        bool WritePlan(std::shared_ptr<CDataSink> sink, const TTravelPlan &plan) override;
};

#endif
