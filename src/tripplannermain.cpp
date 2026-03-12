#include "TextTripPlanWriter.h"
#include "HTMLTripPlanWriter.h"
#include "TripPlannerCommandLine.h"
#include "OpenStreetMap.h"
#include "XMLBusSystem.h"
#include "FileDataFactory.h"
#include "StandardDataSource.h"
#include "StandardDataSink.h"
#include "StandardErrorDataSink.h"
#include <iostream>

class CArgumentParser{
    private:
        std::string DDataDirectory;
        std::string DResultsDirectory;
        bool DArgumentsValid;

        void PrintSyntax() const;
    public:
        CArgumentParser(const std::vector<std::string> &args);

        bool ArgumentsValid() const;

        std::string DataDirectory() const;
        std::string ResultsDirectory() const;
};

int main(int argc, char *argv[]){
    std::vector<std::string> Arguments;
    const std::string OSMFilename = "city.osm";
    const std::string RouteFilename = "busroutes.xml";
    const std::string PathFilename = "busstoppaths.xml";

    // Skip program name
    for(int Index = 1; Index < argc; Index++){
        Arguments.push_back(argv[Index]);
    }

    CArgumentParser Parser(Arguments);
    if(!Parser.ArgumentsValid()){
        return EXIT_FAILURE;
    }
    auto DataFactory = std::make_shared<CFileDataFactory>(Parser.DataDirectory());
    auto ResultsFactory = std::make_shared<CFileDataFactory>(Parser.ResultsDirectory());
    auto StdIn = std::make_shared<CStandardDataSource>();
    auto StdOut = std::make_shared<CStandardDataSink>();
    auto StdErr = std::make_shared<CStandardErrorDataSink>();
    auto RouteReader = std::make_shared<CXMLReader>(DataFactory->CreateSource(RouteFilename));
    auto PathRouter = std::make_shared<CXMLReader>(DataFactory->CreateSource(PathFilename));
    auto BusSystem = std::make_shared<CXMLBusSystem>(RouteReader, PathRouter);
    auto XMLReader = std::make_shared<CXMLReader>(DataFactory->CreateSource(OSMFilename));
    auto StreetMap = std::make_shared<COpenStreetMap>(XMLReader);
    auto TripPlanner = std::make_shared<CTripPlanner>(BusSystem);
    auto OutTripPlanWriter = std::make_shared<CTextTripPlanWriter>(BusSystem);
    auto StorageTripPlanWriter = std::make_shared<CHTMLTripPlanWriter>(StreetMap,BusSystem);

    auto Config = std::make_shared<CTripPlannerCommandLine::SConfig>(StdIn,
                                                                     StdOut,
                                                                     StdErr,
                                                                     ResultsFactory,
                                                                     TripPlanner,
                                                                     StreetMap,
                                                                     OutTripPlanWriter,
                                                                     StorageTripPlanWriter);
    CTripPlannerCommandLine CommandLine(Config);
    if(!CommandLine.ProcessCommands()){
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

CArgumentParser::CArgumentParser(const std::vector<std::string> &args){
    DDataDirectory = "./data";
    DResultsDirectory = "./results";
    DArgumentsValid = true;
    for(size_t Index = 0; Index < args.size(); Index++){
        auto Argument = args[Index];
        if(Argument == "--data"){
            if(Index + 1 >= args.size()){
                DArgumentsValid = false;
                return;
            }
            DDataDirectory = args[Index+1];
            Index++;
        }
        else if(Argument == "--results"){
            if(Index + 1 >= args.size()){
                DArgumentsValid = false;
                return;
            }
            DResultsDirectory = args[Index+1];
            Index++;
        }
        else{
            DArgumentsValid = false;
            break;
        }
    }
    if(!DArgumentsValid){
        PrintSyntax();
    }
}

void CArgumentParser::PrintSyntax() const{
    std::cerr<<"Syntax Error: tripplanner [--data path | --results path]"<<std::endl;
}

bool CArgumentParser::ArgumentsValid() const{
    return DArgumentsValid;
}

std::string CArgumentParser::DataDirectory() const{
    return DDataDirectory;
}

std::string CArgumentParser::ResultsDirectory() const{
    return DResultsDirectory;
}
