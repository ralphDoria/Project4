#include "HTMLTripPlanWriter.h"

#include "SVGTripPlanWriter.h"
#include "StringDataSink.h"

#include <chrono>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

struct CHTMLTripPlanWriter::SImplementation{
    std::shared_ptr<CStreetMap> DStreetMap;
    std::shared_ptr<CBusSystem> DBusSystem;
    std::shared_ptr<CSVGTripPlanWriter> DSVGWriter;

    SImplementation(std::shared_ptr<CStreetMap> streetmap, std::shared_ptr<CBusSystem> bussystem)
        : DStreetMap(std::move(streetmap)),
          DBusSystem(std::move(bussystem)),
          DSVGWriter(std::make_shared<CSVGTripPlanWriter>(DStreetMap, DBusSystem)){
    }

    static bool WriteString(const std::shared_ptr<CDataSink> &sink, const std::string &str){
        if(!sink){
            return false;
        }
        std::vector<char> Buffer(str.begin(), str.end());
        return sink->Write(Buffer);
    }

    static std::string EscapeHTML(const std::string &input){
        std::string Result;
        for(char Ch : input){
            switch(Ch){
                case '&': Result += "&amp;"; break;
                case '<': Result += "&lt;"; break;
                case '>': Result += "&gt;"; break;
                case '"': Result += "&quot;"; break;
                case '\'': Result += "&#39;"; break;
                default: Result += Ch; break;
            }
        }
        return Result;
    }

    static std::string FormatTime(const CBusSystem::TStopTime &time){
        auto MinutesFromMidnight = std::chrono::duration_cast<std::chrono::minutes>(time.to_duration()).count();
        if(MinutesFromMidnight < 0){
            MinutesFromMidnight = 0;
        }

        int TotalMinutes = static_cast<int>(MinutesFromMidnight);
        int Hour24 = (TotalMinutes / 60) % 24;
        int Minute = TotalMinutes % 60;

        std::string Suffix = (Hour24 >= 12) ? "PM" : "AM";
        int Hour12 = Hour24 % 12;
        if(Hour12 == 0){
            Hour12 = 12;
        }

        std::ostringstream Out;
        Out << Hour12 << ":";
        if(Minute < 10){
            Out << "0";
        }
        Out << Minute << " " << Suffix;
        return Out.str();
    }

    std::string StopDescription(CBusSystem::TStopID stopid) const{
        auto Stop = DBusSystem->StopByID(stopid);
        if(!Stop){
            return "";
        }
        return Stop->Description();
    }

    std::string BuildInstructionHTML(const TTravelPlan &plan) const{
        std::ostringstream Out;
        Out << "<div class=\"directions\">";

        for(std::size_t Index = 0; Index < plan.size(); Index++){
            Out << "<div class=\"step\">";

            if(Index == 0){
                Out << "<div class=\"time\">" << EscapeHTML(FormatTime(plan[Index].DTime)) << "</div>";
                Out << "<div>"
                    << "Take the " << EscapeHTML(plan[Index].DRouteName)
                    << " bus from " << EscapeHTML(StopDescription(plan[Index].DStopID))
                    << " (stop " << plan[Index].DStopID << ")."
                    << "</div>";
            }
            else if(Index + 1 == plan.size()){
                Out << "<div class=\"time\">" << EscapeHTML(FormatTime(plan[Index].DTime)) << "</div>";
                Out << "<div>"
                    << "Get off the " << EscapeHTML(plan[Index - 1].DRouteName)
                    << " bus at " << EscapeHTML(StopDescription(plan[Index].DStopID))
                    << " (stop " << plan[Index].DStopID << ")."
                    << "</div>";
            }
            else{
                Out << "<div class=\"time\"></div>";
                Out << "<div>"
                    << "Get off the " << EscapeHTML(plan[Index - 1].DRouteName)
                    << " bus at " << EscapeHTML(StopDescription(plan[Index].DStopID))
                    << " (stop " << plan[Index].DStopID << ") and wait for the "
                    << EscapeHTML(plan[Index].DRouteName) << " bus."
                    << "</div>";

                Out << "</div>";
                Out << "<div class=\"step\">";
                Out << "<div class=\"time\">" << EscapeHTML(FormatTime(plan[Index].DTime)) << "</div>";
                Out << "<div>"
                    << "Take the " << EscapeHTML(plan[Index].DRouteName)
                    << " bus from " << EscapeHTML(StopDescription(plan[Index].DStopID))
                    << " (stop " << plan[Index].DStopID << ")."
                    << "</div>";
            }

            Out << "</div>";
        }

        Out << "</div>";
        return Out.str();
    }
};

CHTMLTripPlanWriter::CHTMLTripPlanWriter(std::shared_ptr<CStreetMap> streetmap, std::shared_ptr<CBusSystem> bussystem)
    : DImplementation(std::make_unique<SImplementation>(std::move(streetmap), std::move(bussystem))){
}

CHTMLTripPlanWriter::~CHTMLTripPlanWriter() = default;

std::shared_ptr<CTripPlanWriter::SConfig> CHTMLTripPlanWriter::Config() const{
    return DImplementation->DSVGWriter->Config();
}

bool CHTMLTripPlanWriter::WritePlan(std::shared_ptr<CDataSink> sink, const TTravelPlan &plan){
    if(!sink || plan.size() < 2){
        return false;
    }

    auto SVGDataSink = std::make_shared<CStringDataSink>();
    if(!DImplementation->DSVGWriter->WritePlan(SVGDataSink, plan)){
        return false;
    }

    std::ostringstream HTML;
    HTML << "<!DOCTYPE html>\n"
         << "<html>\n"
         << "<head>\n"
         << "<meta charset=\"utf-8\" />\n"
         << "<title>Trip Plan</title>\n"
         << "</head>\n"
         << "<body>\n"
         << DImplementation->BuildInstructionHTML(plan) << "\n"
         << SVGDataSink->String() << "\n"
         << "</body>\n"
         << "</html>\n";

    return SImplementation::WriteString(sink, HTML.str());
}