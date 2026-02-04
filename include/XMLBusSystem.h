#ifndef XMLBUSROUTE_H
#define XMLBUSROUTE_H

#include "BusSystem.h"
#include "XMLReader.h"

class CXMLBusSystem : public CBusSystem{
    private:
        struct SImplementation;
        std::unique_ptr< SImplementation > DImplementation;
    public:
        CXMLBusSystem(std::shared_ptr< CXMLReader > systemsource, std::shared_ptr< CXMLReader > pathsource);
        ~CXMLBusSystem();

        std::size_t StopCount() const noexcept override;
        std::size_t RouteCount() const noexcept override;
        std::shared_ptr<SStop> StopByIndex(std::size_t index) const noexcept override;
        std::shared_ptr<SStop> StopByID(TStopID id) const noexcept override;
        std::shared_ptr<SRoute> RouteByIndex(std::size_t index) const noexcept override;
        std::shared_ptr<SRoute> RouteByName(const std::string &name) const noexcept override;
        std::shared_ptr<SPath> PathByStopIDs(TStopID start, TStopID end) const noexcept override;
};

#endif
