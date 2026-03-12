#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "MockBusSystem.h"
#include "MockStreetMap.h"
#include "StringDataSink.h"
#include "StringDataSource.h"
#include "XMLReader.h"
#include "SVGTripPlanWriter.h"
#include <unordered_set>

#include <iostream>
using std::cout;
using std::endl;

class SVGTripPlanWriter : public ::testing::Test{
    protected:
        std::shared_ptr<CMockStreetMap> DStreetMap;
        std::shared_ptr<CMockBusSystem> DBusSystem;
        std::shared_ptr<CStringDataSink> DDataSink;
        std::shared_ptr<CSVGTripPlanWriter> DPlanWriter;
        void SetUp() override{
            DStreetMap = CMockStreetMap::CreateTestStreetMap();
            DBusSystem = CMockBusSystem::CreateTestBusSystem();
            DDataSink = std::make_shared<CStringDataSink>();
            DPlanWriter = std::make_shared<CSVGTripPlanWriter>(DStreetMap, DBusSystem);

        }

        void TearDown() override{
            
        }

};


TEST_F(SVGTripPlanWriter, SimpleRoute){
    CTripPlanner::TTravelPlan TravelPlan;
    TravelPlan.push_back({CBusSystem::TStopTime(std::chrono::hours(8)),28,"F"});
    TravelPlan.push_back({CBusSystem::TStopTime(std::chrono::hours(8) + std::chrono::minutes(25)),82,""});
    
    int HorizontalLines = 0, VerticalLines = 0, StopCircles = 0, StreetLines = 0, BusLines = 0;
    EXPECT_TRUE(DPlanWriter->WritePlan(DDataSink,TravelPlan));
    auto PlanString = DDataSink->String();
    auto SVGStringSource = std::make_shared<CStringDataSource>(PlanString);
    auto XMLReader = std::make_shared<CXMLReader>(SVGStringSource);
    std::unordered_set<std::string> Labels;
    SXMLEntity TempEntity;
    EXPECT_TRUE(XMLReader->ReadEntity(TempEntity,true));
    EXPECT_EQ(TempEntity.DType,SXMLEntity::EType::StartElement);
    EXPECT_EQ(TempEntity.DNameData,"svg");
    while(XMLReader->ReadEntity(TempEntity,true)){
        if(TempEntity.DType == SXMLEntity::EType::StartElement){
            if(TempEntity.DNameData == "line"){
                auto X1 = TempEntity.AttributeValue("x1");
                auto X2 = TempEntity.AttributeValue("x2");
                auto Y1 = TempEntity.AttributeValue("y1");
                auto Y2 = TempEntity.AttributeValue("y2");
                auto Style = TempEntity.AttributeValue("style");
                if(!X1.empty() && !X2.empty() && X1 == X2){
                    VerticalLines++;
                }
                if(!Y1.empty() && !Y2.empty() && Y1 == Y2){
                    HorizontalLines++;
                }
                if(Style.find("stroke:#B0B0B0") != std::string::npos){
                    StreetLines++;
                }
                else if(Style.find("stroke:#8E24AA")!= std::string::npos){
                    BusLines++;
                }
            }
            else if(TempEntity.DNameData == "circle"){
                StopCircles++;
            }
            else if(TempEntity.DNameData == "text"){
                EXPECT_TRUE(XMLReader->ReadEntity(TempEntity));
                EXPECT_EQ(TempEntity.DType,SXMLEntity::EType::CharData);
                Labels.insert(TempEntity.DNameData);
            }
        }

    }
    EXPECT_EQ(HorizontalLines,92);
    EXPECT_EQ(VerticalLines,123);
    EXPECT_EQ(StreetLines,201);
    EXPECT_EQ(BusLines,14);
    EXPECT_EQ(StopCircles,3);
    EXPECT_TRUE(Labels.contains("3rd & K St."));
    EXPECT_TRUE(Labels.contains("6th & F St."));
    EXPECT_TRUE(Labels.contains("9th & C St."));
}

TEST_F(SVGTripPlanWriter, OneStopRoute){
    CTripPlanner::TTravelPlan TravelPlan;
    TravelPlan.push_back({CBusSystem::TStopTime(std::chrono::hours(8)),31,"G"});
    TravelPlan.push_back({CBusSystem::TStopTime(std::chrono::hours(8) + std::chrono::minutes(15)),28,"F"});
    TravelPlan.push_back({CBusSystem::TStopTime(std::chrono::hours(8) + std::chrono::minutes(25)),82,""});
    
    auto Config = DPlanWriter->Config();
    EXPECT_NE(Config,nullptr);

    int HorizontalLines = 0, VerticalLines = 0, StopCircles = 0, StreetLines = 0, BusLines = 0;
    EXPECT_TRUE(DPlanWriter->WritePlan(DDataSink,TravelPlan));
    auto PlanString = DDataSink->String();
    auto SVGStringSource = std::make_shared<CStringDataSource>(PlanString);
    auto XMLReader = std::make_shared<CXMLReader>(SVGStringSource);
    std::unordered_set<std::string> Labels;
    SXMLEntity TempEntity;
    EXPECT_TRUE(XMLReader->ReadEntity(TempEntity,true));
    EXPECT_EQ(TempEntity.DType,SXMLEntity::EType::StartElement);
    EXPECT_EQ(TempEntity.DNameData,"svg");
    while(XMLReader->ReadEntity(TempEntity,true)){
        if(TempEntity.DType == SXMLEntity::EType::StartElement){
            if(TempEntity.DNameData == "line"){
                auto X1 = TempEntity.AttributeValue("x1");
                auto X2 = TempEntity.AttributeValue("x2");
                auto Y1 = TempEntity.AttributeValue("y1");
                auto Y2 = TempEntity.AttributeValue("y2");
                auto Style = TempEntity.AttributeValue("style");
                if(!X1.empty() && !X2.empty() && X1 == X2){
                    VerticalLines++;
                }
                if(!Y1.empty() && !Y2.empty() && Y1 == Y2){
                    HorizontalLines++;
                }
                if(Style.find("stroke:#B0B0B0") != std::string::npos){
                    StreetLines++;
                }
                else if((Style.find("stroke:#8E24AA")!= std::string::npos)||(Style.find("stroke:#F57C00")!= std::string::npos)){
                    BusLines++;
                }
            }
            else if(TempEntity.DNameData == "circle"){
                StopCircles++;
            }
            else if(TempEntity.DNameData == "text"){
                EXPECT_TRUE(XMLReader->ReadEntity(TempEntity));
                EXPECT_EQ(TempEntity.DType,SXMLEntity::EType::CharData);
                Labels.insert(TempEntity.DNameData);
            }
        }

    }
    EXPECT_EQ(HorizontalLines,118);
    EXPECT_EQ(VerticalLines,125);
    EXPECT_EQ(StreetLines,226);
    EXPECT_EQ(BusLines,17);
    EXPECT_EQ(StopCircles,4);
    EXPECT_TRUE(Labels.contains("1st & J St."));
    EXPECT_TRUE(Labels.contains("3rd & K St."));
    EXPECT_TRUE(Labels.contains("6th & F St."));
    EXPECT_TRUE(Labels.contains("9th & C St."));
}

TEST_F(SVGTripPlanWriter, ConfigTest){    
    auto Config = DPlanWriter->Config();
    auto ValidOptions = Config->ValidOptions();
    auto ValidFlags = Config->ValidFlags();
    
    EXPECT_TRUE(ValidOptions.contains(std::string(CSVGTripPlanWriter::BusColor0)));
    EXPECT_TRUE(ValidFlags.contains(std::string(CSVGTripPlanWriter::PrimaryEnabled)));

    EXPECT_FALSE(ValidOptions.contains("bad"));
    EXPECT_FALSE(ValidFlags.contains("bad"));

    EXPECT_TRUE(Config->FlagEnabled(CSVGTripPlanWriter::MotorwayEnabled));
    EXPECT_TRUE(Config->FlagEnabled(CSVGTripPlanWriter::PrimaryEnabled));
    EXPECT_TRUE(Config->FlagEnabled(CSVGTripPlanWriter::SecondaryEnabled));
    EXPECT_TRUE(Config->FlagEnabled(CSVGTripPlanWriter::TertiaryEnabled));
    EXPECT_TRUE(Config->FlagEnabled(CSVGTripPlanWriter::ResidentialEnabled));
    EXPECT_EQ(Config->GetOptionType(CSVGTripPlanWriter::BusColor0),CTripPlanWriter::SConfig::EOptionType::String);
    EXPECT_EQ(Config->GetOptionType(CSVGTripPlanWriter::BusColor1),CTripPlanWriter::SConfig::EOptionType::String);
    EXPECT_EQ(Config->GetOptionType(CSVGTripPlanWriter::BusStopRadius),CTripPlanWriter::SConfig::EOptionType::Double);
    EXPECT_EQ(Config->GetOptionType(CSVGTripPlanWriter::BusStroke),CTripPlanWriter::SConfig::EOptionType::Int);
    EXPECT_EQ(Config->GetOptionType(CSVGTripPlanWriter::DestinationColor),CTripPlanWriter::SConfig::EOptionType::String);
    EXPECT_EQ(Config->GetOptionType(CSVGTripPlanWriter::DestinationRadius),CTripPlanWriter::SConfig::EOptionType::Double);
    EXPECT_EQ(Config->GetOptionType(CSVGTripPlanWriter::LabelBackground),CTripPlanWriter::SConfig::EOptionType::String);
    EXPECT_EQ(Config->GetOptionType(CSVGTripPlanWriter::LabelColor),CTripPlanWriter::SConfig::EOptionType::String);
    EXPECT_EQ(Config->GetOptionType(CSVGTripPlanWriter::LabelMargin),CTripPlanWriter::SConfig::EOptionType::Int);
    EXPECT_EQ(Config->GetOptionType(CSVGTripPlanWriter::LabelPaintOrder),CTripPlanWriter::SConfig::EOptionType::String);
    EXPECT_EQ(Config->GetOptionType(CSVGTripPlanWriter::LabelSize),CTripPlanWriter::SConfig::EOptionType::Int);
    EXPECT_EQ(Config->GetOptionType(CSVGTripPlanWriter::MotorwayStroke),CTripPlanWriter::SConfig::EOptionType::Int);
    EXPECT_EQ(Config->GetOptionType(CSVGTripPlanWriter::PrimaryStroke),CTripPlanWriter::SConfig::EOptionType::Int);
    EXPECT_EQ(Config->GetOptionType(CSVGTripPlanWriter::ResidentialStroke),CTripPlanWriter::SConfig::EOptionType::Int);
    EXPECT_EQ(Config->GetOptionType(CSVGTripPlanWriter::SecondaryStroke),CTripPlanWriter::SConfig::EOptionType::Int);
    EXPECT_EQ(Config->GetOptionType(CSVGTripPlanWriter::SourceColor),CTripPlanWriter::SConfig::EOptionType::String);
    EXPECT_EQ(Config->GetOptionType(CSVGTripPlanWriter::SourceRadius),CTripPlanWriter::SConfig::EOptionType::Double);
    EXPECT_EQ(Config->GetOptionType(CSVGTripPlanWriter::StreetColor),CTripPlanWriter::SConfig::EOptionType::String);
    EXPECT_EQ(Config->GetOptionType(CSVGTripPlanWriter::SVGHeight),CTripPlanWriter::SConfig::EOptionType::Int);
    EXPECT_EQ(Config->GetOptionType(CSVGTripPlanWriter::SVGMarginPixels),CTripPlanWriter::SConfig::EOptionType::Int);
    EXPECT_EQ(Config->GetOptionType(CSVGTripPlanWriter::SVGWidth),CTripPlanWriter::SConfig::EOptionType::Int);
    EXPECT_EQ(Config->GetOptionType(CSVGTripPlanWriter::TertiaryStroke),CTripPlanWriter::SConfig::EOptionType::Int);
    
    EXPECT_EQ(std::any_cast<std::string>(Config->GetOption(CSVGTripPlanWriter::BusColor0)),"#8E24AA" );
    EXPECT_EQ(std::any_cast<std::string>(Config->GetOption(CSVGTripPlanWriter::BusColor1)),"#F57C00" );
    EXPECT_EQ(std::any_cast<double>(Config->GetOption(CSVGTripPlanWriter::BusStopRadius)), 8.0);
    EXPECT_EQ(std::any_cast<int>(Config->GetOption(CSVGTripPlanWriter::BusStroke)), 8);
    EXPECT_EQ(std::any_cast<std::string>(Config->GetOption(CSVGTripPlanWriter::DestinationColor)), "#FF0000");
    EXPECT_EQ(std::any_cast<double>(Config->GetOption(CSVGTripPlanWriter::DestinationRadius)), 8.0);
    EXPECT_EQ(std::any_cast<std::string>(Config->GetOption(CSVGTripPlanWriter::LabelBackground)), "#FFFFFF80");
    EXPECT_EQ(std::any_cast<std::string>(Config->GetOption(CSVGTripPlanWriter::LabelColor)), "#000000");
    EXPECT_EQ(std::any_cast<int>(Config->GetOption(CSVGTripPlanWriter::LabelMargin)), 8);
    EXPECT_EQ(std::any_cast<std::string>(Config->GetOption(CSVGTripPlanWriter::LabelPaintOrder)), "stroke fill");
    EXPECT_EQ(std::any_cast<int>(Config->GetOption(CSVGTripPlanWriter::LabelSize)), 16);
    EXPECT_EQ(std::any_cast<int>(Config->GetOption(CSVGTripPlanWriter::MotorwayStroke)), 6);
    EXPECT_EQ(std::any_cast<int>(Config->GetOption(CSVGTripPlanWriter::PrimaryStroke)), 4);
    EXPECT_EQ(std::any_cast<int>(Config->GetOption(CSVGTripPlanWriter::ResidentialStroke)),2 );
    EXPECT_EQ(std::any_cast<int>(Config->GetOption(CSVGTripPlanWriter::SecondaryStroke)), 2);
    EXPECT_EQ(std::any_cast<std::string>(Config->GetOption(CSVGTripPlanWriter::SourceColor)), "#00FF00");
    EXPECT_EQ(std::any_cast<double>(Config->GetOption(CSVGTripPlanWriter::SourceRadius)), 8.0);
    EXPECT_EQ(std::any_cast<std::string>(Config->GetOption(CSVGTripPlanWriter::StreetColor)), "#B0B0B0");
    EXPECT_EQ(std::any_cast<int>(Config->GetOption(CSVGTripPlanWriter::SVGHeight)), 540);
    EXPECT_EQ(std::any_cast<int>(Config->GetOption(CSVGTripPlanWriter::SVGMarginPixels)), 30);
    EXPECT_EQ(std::any_cast<int>(Config->GetOption(CSVGTripPlanWriter::SVGWidth)), 960);
    EXPECT_EQ(std::any_cast<int>(Config->GetOption(CSVGTripPlanWriter::TertiaryStroke)), 2);


    EXPECT_EQ(Config->GetOptionType("bad"),CTripPlanWriter::SConfig::EOptionType::None);
    EXPECT_FALSE(Config->GetOption("bad").has_value());
    Config->ClearOption("bad");
    Config->DisableFlag(CSVGTripPlanWriter::ResidentialEnabled);
    EXPECT_FALSE(Config->FlagEnabled(CSVGTripPlanWriter::ResidentialEnabled));
}

TEST_F(SVGTripPlanWriter, UpdatedDescription){    
    auto Stop = DBusSystem->StopByID(99);
    EXPECT_EQ(Stop->Description(),"1st & M St.");
}