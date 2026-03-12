#ifndef TRIPPLANNERCOMMANDLINE_H
#define TRIPPLANNERCOMMANDLINE_H

#include "DataFactory.h"
#include "TripPlanner.h"
#include "TripPlanWriter.h"
#include <memory>
#include <vector>

class CTripPlannerCommandLine{
    private:
        struct SImplementation;
        std::unique_ptr<SImplementation> DImplementation;
    public:
        struct SConfig{
            std::shared_ptr<CDataSource> DCommandSource;
            std::shared_ptr<CDataSink> DOutSink;
            std::shared_ptr<CDataSink> DErrorSink;
            std::shared_ptr<CDataFactory> DResultsFactory;
            std::shared_ptr<CTripPlanner> DTripPlanner;
            std::shared_ptr<CStreetMap> DStreetMap;
            std::shared_ptr<CTripPlanWriter> DOutWriter;
            std::shared_ptr<CTripPlanWriter> DStorageWriter;

            SConfig(std::shared_ptr<CDataSource> commandsource,
                    std::shared_ptr<CDataSink> outsink,
                    std::shared_ptr<CDataSink> errorsink,
                    std::shared_ptr<CDataFactory> resultsfactory,
                    std::shared_ptr<CTripPlanner> tripplanner,
                    std::shared_ptr<CStreetMap> streetmap,
                    std::shared_ptr<CTripPlanWriter> outwriter,
                    std::shared_ptr<CTripPlanWriter> storagewriter)
                : DCommandSource(std::move(commandsource)),
                DOutSink(std::move(outsink)),
                DErrorSink(std::move(errorsink)),
                DResultsFactory(std::move(resultsfactory)),
                DTripPlanner(std::move(tripplanner)),
                DStreetMap(std::move(streetmap)),
                DOutWriter(std::move(outwriter)),
                DStorageWriter(std::move(storagewriter)){
            }
        };

        CTripPlannerCommandLine(std::shared_ptr<SConfig> config);
        ~CTripPlannerCommandLine();
        bool ProcessCommands();
};

#endif
