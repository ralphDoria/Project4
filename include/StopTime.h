/*

#ifndef STOPTIME_H
#define STOPTIME_H

#include <algorithm>
#include <string>
#include <compare>
#include <concepts>
#include <ostream>
#include <sstream>
#include <iomanip>

class CDeltaTime{
    private:
        int DDeltaTime;

    public:
        constexpr CDeltaTime(int deltattime = 0) noexcept : DDeltaTime(deltattime){
        }

        constexpr CDeltaTime(const CDeltaTime &deltattime) = default;
        constexpr CDeltaTime(CDeltaTime&&) = default;

        constexpr operator int() const{
            return DDeltaTime;
        }

        constexpr CDeltaTime &operator=(const CDeltaTime &deltatime) = default;
        constexpr CDeltaTime &operator=(CDeltaTime &&deltatime) = default;

        constexpr auto operator<=>(const CDeltaTime&) const = default;

        constexpr CDeltaTime &operator+=(const CDeltaTime &deltatime) noexcept{
            DDeltaTime += deltatime.DDeltaTime;
            return *this;
        }

        constexpr CDeltaTime &operator-=(const CDeltaTime &deltatime){
            DDeltaTime -= deltatime.DDeltaTime;
            return *this;
        }

        CDeltaTime operator+(const CDeltaTime &deltattime) const{
            return {DDeltaTime + deltattime.DDeltaTime};
        }

        CDeltaTime operator-(const CDeltaTime &deltattime) const{
            return {DDeltaTime - deltattime.DDeltaTime};
        }

        CDeltaTime operator-() const{
            return {-DDeltaTime};
        }

        // Scaling (duration * scalar)
        template<std::floating_point T>
        constexpr CDeltaTime operator*(T scalar) const noexcept{
            return CDeltaTime{DMinutes * static_cast<int>(scalar)};
        }

        template<std::floating_point T>
        constexpr CDeltaTime operator/(T scalar) const noexcept{
            return CDeltaTime{DMinutes / static_cast<int>(scalar)};
        }

        template<std::floating_point T>
        constexpr CDeltaTime &operator*=(T scalar) noexcept{
            DMinutes *= static_cast<int>(scalar);
            return *this;
        }

        template<std::floating_point T>
        constexpr CDeltaTime &operator/=(T scalar) noexcept{
            DMinutes /= static_cast<int>(scalar);
            return *this;
        }

        // Comparison (C++20 spaceship)
        constexpr auto operator<=>(const CDeltaTime&) const = default;

        // Output
        friend std::ostream &operator<<(std::ostream &os, const CDeltaTime &value){
            return os << value.DDeltaTime << " min";
        }

        // Symmetric scaling (scalar * duration)
        template<std::floating_point T>
        friend constexpr CDeltaTime operator*(T scalar, const CDeltaTime &Value) noexcept{
            return Value * scalar;
        }
};


class CStopTime{
    private:
        int DTime;
        inline static constexpr int MINUTES_PER_DAY = 1440;
        inline static constexpr int MINUTES_PER_HOUR = 60;
        inline static constexpr char SEPARATOR_CHAR = ':';
        inline static constexpr std::string SUFFIX_AM = "AM";
        inline static constexpr std::string SUFFIX_PM = "PM";


        constexpr void Normalize() noexcept {
            DTime %= MINUTES_PER_DAY;
            while(DTime < 0){
                DTime += MINUTES_PER_DAY;
            }
        }

    public:
        constexpr CStopTime() noexcept : DTime(0) {}
        constexpr CStopTime(int minsincemidnight) noexcept : DTime(minsincemidnight) { Normalize(); }
        constexpr CStopTime(const CStopTime&) = default;

        CStopTime(const std::string &timestr) {
            std::string TempString = timestr;
            std::transform(TempString.begin(), TempString.end(), TempString.begin(), ::toupper); // normalize

            int Hour = 0, Minute = 0;
            char Separator = '\0';
            bool Is12Hour = false;
            bool IsPM = false;

            std::istringstream InputStringStream(TempString);
            InputStringStream >> Hour >> Separator >> Minute;
            if(InputStringStream.fail() || Separator != SEPARATOR_CHAR){
                throw std::invalid_argument("Invalid time format");
            }

            std::string suffix;
            InputStringStream >> suffix;

            if (!suffix.empty()){
                Is12Hour = true;
                if(suffix == SUFFIX_PM){
                    IsPM = true;
                }
                else if (suffix != SUFFIX_AM){
                    throw std::invalid_argument("Invalid AM/PM specifier");
                }
            }

            if(Is12Hour){
                if (Hour < 1 || Hour > 12 || Minute < 0 || Minute > 59)
                    throw std::invalid_argument("Invalid 12-hour time");
                if(Hour == 12){
                    Hour = 0;        // 12 AM → 0, 12 PM → 12 handled below
                }
                if(IsPM){
                    Hour += 12;
                }
            } 
            else {
                if (Hour < 0 || Hour > 23 || Minute < 0 || Minute > 59){
                    throw std::invalid_argument("Invalid 24-hour time");
                }
            }

            DTime = Hour * MINUTES_PER_HOUR + Minute;
            Normalize();
        }
        explicit CStopTime(double Minutes) : DTime(static_cast<int>(Minutes)) { Normalize(); }

        // Assignment
        CStopTime &operator=(const CStopTime&) = default;

        // Accessor
        constexpr operator int() const{
            return DTime;
        }

        // Arithmetic with DeltaTime
        constexpr CStopTime operator+(const CDeltaTime &deltatime) const noexcept {
            return CStopTime{DTime + int(deltatime)};
        }

        constexpr CStopTime operator-(const CDeltaTime &deltatime) const noexcept {
            return CStopTime{DTime - int(deltatime)};
        }

        // StopTime difference → DeltaTime
        constexpr CDeltaTime operator-(const CStopTime &other) const noexcept {
            return CDeltaTime{DTime - other.DTime};
        }

        // Comparison
        constexpr auto operator<=>(const CStopTime&) const = default;

        // Output
        friend std::ostream &operator<<(std::ostream &os, const CStopTime &stoptime) {
            int Hour = stoptime.DTime / CStopTime::MINUTES_PER_HOUR;
            int Minute = stoptime.DTime % CStopTime::MINUTES_PER_HOUR;
            os << std::setw(2) << std::setfill('0') << Hour
            << CStopTime::SEPARATOR_CHAR << std::setw(2) << std::setfill('0') << Minute;
            return OS;
        }

};

#endif
*/