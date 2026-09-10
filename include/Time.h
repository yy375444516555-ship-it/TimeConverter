#pragma once

#include "Scale.h"

#include <cmath>
#include <compare>
#include <ostream>
#include <stdexcept>

template<Scale scale>
class Time {
private:
    double jdInt_;
    double jdFrac_;

public:
    Time(double jd1 = 0.0, double jd2 = 0.0) noexcept {
        jd2 += jd1 - std::floor(jd1);
        jd1 = std::floor(jd1);

        const double c = std::floor(jd2);
        jdInt_ = jd1 + c;
        jdFrac_ = jd2 - c;
    }

    auto operator<=>(const Time& other) const noexcept = default;

    static Time fromJD(double jd) noexcept {
        return Time(jd, 0.0);
    }

    static Time fromMJD(double mjd) noexcept {
        return Time(mjd + 2400000.5, 0.0);
    }

    static Time fromCalendar(
        int year,
        int month,
        int day,
        int hour = 0,
        int minute = 0,
        double second = 0.0
    ) {
        if (month < 1 || month > 12) {
            throw std::invalid_argument("Invalid month");
        }

        // Проверка високосного года.
        const bool leap =
            (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);

        const int daysInMonth[] = {
            31,
            leap ? 29 : 28,
            31,
            30,
            31,
            30,
            31,
            31,
            30,
            31,
            30,
            31
        };

        if (day < 1 || day > daysInMonth[month - 1]) {
            throw std::invalid_argument("Invalid day");
        }

        if (hour < 0 || hour > 23) {
            throw std::invalid_argument("Invalid hour");
        }

        if (minute < 0 || minute > 59) {
            throw std::invalid_argument("Invalid minute");
        }

        if (second < 0.0 || second >= 60.0) {
            throw std::invalid_argument("Invalid second");
        }

        const int a = (14 - month) / 12;
        const int y = year + 4800 - a;
        const int m = month + 12 * a - 3;

        const int jdn =
            day
            + (153 * m + 2) / 5
            + 365 * y
            + y / 4
            - y / 100
            + y / 400
            - 32045;

        const double secondsInDay =
            hour * 3600.0 + minute * 60.0 + second;

        const double dayFraction = secondsInDay / 86400.0;

        return Time(static_cast<double>(jdn), dayFraction - 0.5);
    }

    double getJdInt() const noexcept {
        return jdInt_;
    }

    double getJdFrac() const noexcept {
        return jdFrac_;
    }

    double getJd() const noexcept {
        return jdInt_ + jdFrac_;
    }

    double getMjd() const noexcept {
        return jdInt_ + jdFrac_ - 2400000.5;
    }
};

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
    os << "Jd: " << time.getJdInt() << ' ' << time.getJdFrac() << '\n';
    return os;
}
