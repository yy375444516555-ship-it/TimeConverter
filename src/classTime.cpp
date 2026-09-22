#include "../include/classTime.h"

#include <cmath>
#include <ostream>
#include <stdexcept>
#include <sofa.h>

template<Scale scale>
Time<scale>::Time(double jd1, double jd2) noexcept {
    jd2 += jd1 - std::floor(jd1);
    jd1 = std::floor(jd1);

    const double c = std::floor(jd2);
    jdInt_ = jd1 + c;
    jdFrac_ = jd2 - c;
}

template<Scale scale>
std::partial_ordering Time<scale>::operator<=>(const Time& other) const noexcept {
    if (const auto cmp = jdInt_ <=> other.jdInt_; cmp != 0) {
        return cmp;
    }
    return jdFrac_ <=> other.jdFrac_;
}

template<Scale scale>
bool Time<scale>::operator==(const Time& other) const noexcept {
    return jdInt_ == other.jdInt_ && jdFrac_ == other.jdFrac_;
}

template<Scale scale>
Time<scale> Time<scale>::fromJD(double jd) noexcept {
    return Time(jd, 0.0);
}

template<Scale scale>
Time<scale> Time<scale>::fromMJD(double mjd) noexcept {
    return Time(mjd + 2400000.5, 0.0);
}

template<Scale scale>
Time<scale> Time<scale>::fromCalendar(int year, int month, int day, int hour, int minute, double second) {
    double d1, d2;

    const char* scaleName = nullptr;

    if constexpr (scale == Scale::UTC)
        scaleName = "UTC";
    else if constexpr (scale == Scale::UT1)
        scaleName = "UT1";
    else if constexpr (scale == Scale::TAI)
        scaleName = "TAI";
    else if constexpr (scale == Scale::TT)
        scaleName = "TT";
    else if constexpr (scale == Scale::TCG)
        scaleName = "TCG";
    else if constexpr (scale == Scale::TDB)
        scaleName = "TDB";
    else if constexpr (scale == Scale::TCB)
        scaleName = "TCB";

    int status = iauDtf2d(scaleName, year, month, day, hour, minute, second, &d1, &d2);
    if (status < 0)  throw std::runtime_error("iauDtf2d: unacceptable date");

    return Time<scale>(d1,d2);
}

template<Scale scale>
double Time<scale>::getJdInt() const noexcept {
    return jdInt_;
}

template<Scale scale>
double Time<scale>::getJdFrac() const noexcept {
    return jdFrac_;
}

template<Scale scale>
double Time<scale>::getJd() const noexcept {
    return jdInt_ + jdFrac_;
}

template<Scale scale>
double Time<scale>::getMjd() const noexcept {
    return jdInt_ + jdFrac_ - 2400000.5;
}

template<Scale scale>
double operator-(const Time<scale>& first, const Time<scale>& second) noexcept {
    const double days =
        (first.getJdInt() - second.getJdInt())
        + (first.getJdFrac() - second.getJdFrac());

    return days * 86400.0;
}

template<Scale scale>
Time<scale> operator-(const Time<scale>& time, double secs) noexcept {
    return Time<scale>(
        time.getJdInt(),
        time.getJdFrac() - secs / 86400.0
    );
}

template<Scale scale>
Time<scale> operator+(const Time<scale>& time, double secs) noexcept {
    return Time<scale>(
        time.getJdInt(),
        time.getJdFrac() + secs / 86400.0
    );
}

template<Scale scale>
std::ostream& operator<<(std::ostream& os, const Time<scale>& time) {
    os << "JD: " << time.getJdInt() << ' ' << time.getJdFrac();
    return os;
}

#define INSTANTIATE_TIME(S) \
    template class Time<S>; \
    template double operator-<S>(const Time<S>&, const Time<S>&) noexcept; \
    template Time<S> operator-<S>(const Time<S>&, double) noexcept; \
    template Time<S> operator+<S>(const Time<S>&, double) noexcept; \
    template std::ostream& operator<< <S>(std::ostream&, const Time<S>&)

INSTANTIATE_TIME(Scale::UTC);
INSTANTIATE_TIME(Scale::UT1);
INSTANTIATE_TIME(Scale::TAI);
INSTANTIATE_TIME(Scale::TT);
INSTANTIATE_TIME(Scale::TCG);
INSTANTIATE_TIME(Scale::TCB);
INSTANTIATE_TIME(Scale::TDB);

#undef INSTANTIATE_TIME
