#pragma once

#include "Scale.h"

#include <compare>
#include <iosfwd>

template<Scale scale>
class Time {
private:
    double jdInt_;
    double jdFrac_;

public:
    Time(double jd1 = 0.0, double jd2 = 0.0) noexcept;

    std::partial_ordering operator<=>(const Time& other) const noexcept;
    bool operator==(const Time& other) const noexcept;

    static Time fromJD(double jd) noexcept;
    static Time fromMJD(double mjd) noexcept;
    static Time fromCalendar(int year, int month, int day, int hour = 0, int minute = 0, double second = 0.0);

    double getJdInt() const noexcept;
    double getJdFrac() const noexcept;
    double getJd() const noexcept;
    double getMjd() const noexcept;
};

template<Scale scale>
double operator-(const Time<scale>& first, const Time<scale>& second) noexcept;

template<Scale scale>
Time<scale> operator-(const Time<scale>& time, double secs) noexcept;

template<Scale scale>
Time<scale> operator+(const Time<scale>& time, double secs) noexcept;

template<Scale scale>
std::ostream& operator<<(std::ostream& os, const Time<scale>& time);


extern template class Time<Scale::UTC>;
extern template class Time<Scale::UT1>;
extern template class Time<Scale::TAI>;
extern template class Time<Scale::TT>;
extern template class Time<Scale::TCG>;
extern template class Time<Scale::TCB>;
extern template class Time<Scale::TDB>;
