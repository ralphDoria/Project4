#ifndef TSTOPTIME_H
#define TSTOPTIME_H

//needed in order to set up chrono for STopTime in BusSystem and remove CStopTime weird impl
//this is the main stoptime file we are now using

#include <chrono>

class TStopTime {
public:
    using TDuration = std::chrono::minutes;

private:
    TDuration DSinceMidnight;

public:
    //default
    constexpr TStopTime() noexcept : DSinceMidnight(0) {}

    //construct from a chrono duration
    constexpr explicit TStopTime(TDuration dur) noexcept : DSinceMidnight(dur) {}

    //construct fromhours, seconds, etc.
    template<class Rep, class Period>
    constexpr explicit TStopTime(std::chrono::duration<Rep, Period> dur) noexcept
        : DSinceMidnight(std::chrono::duration_cast<TDuration>(dur)) {}

    constexpr TDuration to_duration() const noexcept {
        return DSinceMidnight;
    }

    //comparisons
    bool operator==(const TStopTime &Other) const noexcept {
        return DSinceMidnight == Other.DSinceMidnight;
    }

    bool operator!=(const TStopTime &Other) const noexcept {
        return !(*this == Other);
    }

    bool operator<(const TStopTime &Other) const noexcept {
        return DSinceMidnight < Other.DSinceMidnight;
    }

    bool operator<=(const TStopTime &Other) const noexcept {
        return !(Other < *this);
    }

    bool operator>(const TStopTime &Other) const noexcept {
        return Other < *this;
    }

    bool operator>=(const TStopTime &Other) const noexcept {
        return !(*this < Other);
    }
};

#endif