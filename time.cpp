#include <iostream>
#include <stdexcept>
#include <cmath>
#include <iomanip>
#include <vector>

enum class Scale{UTC = 0, UT1 = 1, TAI = 2, TT = 3, TCG = 4, TCB = 5, TDB = 6};

template<Scale scale>
class Time{

    private:
        double jdInt_;
        double jdFrac_;

    public:
        Time(double jd1=0., double jd2=0.) noexcept { 

            jd2 += jd1 - std::floor(jd1);
            jd1 = std::floor(jd1);
            
            double c = std::floor(jd2);
            jdInt_ = jd1 + c;
            jdFrac_ = jd2 - c;
        }

        auto operator<=>(const Time& other) const noexcept = default;

        Time static fromJD (double jd) noexcept{
            return Time(jd, 0.);
        }
        Time static fromMJD (double mjd) noexcept{
            return Time(mjd + 2400000.5, 0.);
        }
        Time static fromCalendar(int year, int month, int day, int hour=0, int minute=0, double second=0){
            if (month < 1 || month > 12) {
                throw std::invalid_argument("Invalid month");
            }

            //Проверка високосного года
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

            const double dayFraction =
                secondsInDay / 86400.0;

            return Time(static_cast<double>(jdn), dayFraction - 0.5
            );
        }
        
        void printTime() noexcept {
            std::cout<<"Jd: " << jdInt_ << " " << jdFrac_ << std::endl;
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
    double days = (first.getJdInt() - second.getJdInt()) + (first.getJdFrac() - second.getJdFrac());
    return days * 86400.0;
}
template<Scale scale>
Time<scale> operator-(const Time<scale> &t, double secs) noexcept {
    return Time(t.getJdInt(), t.getJdFrac() - (secs / 86400.0));
}
template<Scale scale>
Time<scale> operator+(const Time<scale> &t, double secs) noexcept {
    return Time(t.getJdInt(), t.getJdFrac() + (secs / 86400.0)); 
}

class SomeDutContainer{
    // Something ...
    public:
        double dut(double mjd) const;
};

class SomeDATContainer{
    // Something ...
    public:
        double dat(double mjd) const;
};

class SomeTcbContainer{
    // Something ...
    public:
        double dtcb(double mjd) const;
};


template<typename DutContainer, typename DatContainer>
class TimeConverter {

    private:
        DutContainer dutContainer_;
        DatContainer datContainer_;
        SomeTcbContainer tcbContainer_;

    public:
        TimeConverter(const DutContainer& dutContainer, const DatContainer& datContainer, const SomeTcbContainer& tcbContainer) 
        : dutContainer_(dutContainer), datContainer_(datContainer), tcbContainer_(tcbContainer){};

        template<Scale To, Scale From>
        Time<To> convert(const Time<From>& from) const {

            if constexpr (To == From) {
                return Time<To>::fromJD(from.getJd());
            }
            
            else if constexpr(From == Scale::UTC && To == Scale::UT1){
                double dut_day = dutContainer_.dut(from.getMjd()) / 86400.0;
                double ut1JdFrac = from.getJdFrac() + dut_day;
                return Time<To>(from.getJdInt(), ut1JdFrac);
            }
            else if constexpr(From == Scale::UT1 && To == Scale::UTC){
                double dut_day = dutContainer_.dut(from.getMjd()) / 86400.0;
                double utcJdFrac = from.getJdFrac() - dut_day;
                return Time<To>(from.getJdInt(), utcJdFrac);
            }

            else if constexpr (From == Scale::TAI && To == Scale::TT){
                return Time<To>(from.getJdInt(), from.getJdFrac() + (32.184 / 86400.0));
            }
            else if constexpr (From == Scale::TT && To == Scale::TAI){
                return Time<To>(from.getJdInt(), from.getJdFrac() - (32.184 / 86400.0));
            }

            else if constexpr (From == Scale::TAI && To == Scale::UTC){
                double dat_day = datContainer_.dat(from.getMjd()) / 86400.0;
                double utcJdFrac = from.getJdFrac() - dat_day;
                return Time<To>(from.getJdInt(), utcJdFrac);
            }
            else if constexpr (From == Scale::UTC && To == Scale::TAI){
                double dat_day = datContainer_.dat(from.getMjd()) / 86400.0;
                double taiJdFrac = from.getJdFrac() + dat_day;
                return Time<To>(from.getJdInt(), taiJdFrac);
            }
            
            else if constexpr (From == Scale::TAI && To == Scale::UT1){
                auto utc = convert<Scale::UTC>(from);
                return convert<Scale::UT1>(utc);
            }
            else if constexpr (From == Scale::UT1 && To == Scale::TAI){
                auto utc = convert<Scale::UTC>(from);
                return convert<Scale::TAI>(utc);
            }

            else if constexpr (From == Scale::UTC && To == Scale::TT){
                auto tai = convert<Scale::TAI>(from);
                return convert<Scale::TT>(tai);
            }
            else if constexpr (From == Scale::TT && To == Scale::UTC){
                auto tai = convert<Scale::TAI>(from);
                return convert<Scale::UTC>(tai);
            }

            else if constexpr (From == Scale::UT1 && To == Scale::TT){
                auto utc = convert<Scale::UTC>(from);
                return convert<Scale::TT>(utc);
            }
            else if constexpr (From == Scale::TT && To == Scale::UT1){
                auto utc = convert<Scale::UTC>(from);
                return convert<Scale::UT1>(utc);
            }

            else if constexpr (From == Scale::TT && To == Scale::TCG){
                const double LG = 6.969290134e-10;
                const double TT0 = 2443144.5003725;

                double deltaJd = from.getJd() - TT0;
                double tcgJdFrac = from.getJdFrac() + LG * deltaJd;

                return Time<To>(from.getJdInt(), tcgJdFrac);
            }

            else if constexpr (From == Scale::TCG && To == Scale::TCB){
                double dtcb_day = tcbContainer_.dtcb(from.getMjd()) / 86400.0;
                double tcbJdFrac = from.getJdFrac() + dtcb_day;

                return Time<To>(from.getJdInt(), tcbJdFrac);
            }

            else if constexpr (From == Scale::TCB && To == Scale::TDB){
                const double LB = 1.550519768e-8;
                const double T0_INT = 2443144.0;
                const double T0_FRAC = 0.5003725;
                const double TDB0 = -6.55e-5;

                double deltaJd = (from.getJdInt() - T0_INT) + (from.getJdFrac() - T0_FRAC);
                double correction_sec = -LB * deltaJd * 86400.0 + TDB0;
                double tdbJdFrac = from.getJdFrac() + correction_sec / 86400.0;

                return Time<To>(from.getJdInt(), tdbJdFrac);
            }

            else if constexpr (From == Scale::TAI && To == Scale::TCG){
                auto tt = convert<Scale::TT>(from);
                return convert<Scale::TCG>(tt);
            }

            else if constexpr (From == Scale::UTC && To == Scale::TCG){
                auto tt = convert<Scale::TT>(from);
                return convert<Scale::TCG>(tt);
            }

            else if constexpr (From == Scale::UT1 && To == Scale::TCG){
                auto tt = convert<Scale::TT>(from);
                return convert<Scale::TCG>(tt);
            }

            else if constexpr (From == Scale::TT && To == Scale::TCB){
                auto tcg = convert<Scale::TCG>(from);
                return convert<Scale::TCB>(tcg);
            }

            else if constexpr (From == Scale::TAI && To == Scale::TCB){
                auto tcg = convert<Scale::TCG>(from);
                return convert<Scale::TCB>(tcg);
            }

            else if constexpr (From == Scale::UTC && To == Scale::TCB){
                auto tcg = convert<Scale::TCG>(from);
                return convert<Scale::TCB>(tcg);
            }

            else if constexpr (From == Scale::UT1 && To == Scale::TCB){
                auto tcg = convert<Scale::TCG>(from);
                return convert<Scale::TCB>(tcg);
            }

            else if constexpr (From == Scale::TCG && To == Scale::TDB){
                auto tcb = convert<Scale::TCB>(from);
                return convert<Scale::TDB>(tcb);
            }

            else if constexpr (From == Scale::TT && To == Scale::TDB){
                auto tcb = convert<Scale::TCB>(from);
                return convert<Scale::TDB>(tcb);
            }

            else if constexpr (From == Scale::TAI && To == Scale::TDB){
                auto tcb = convert<Scale::TCB>(from);
                return convert<Scale::TDB>(tcb);
            }

            else if constexpr (From == Scale::UTC && To == Scale::TDB){
                auto tcb = convert<Scale::TCB>(from);
                return convert<Scale::TDB>(tcb);
            }

            else if constexpr (From == Scale::UT1 && To == Scale::TDB){
                auto tcb = convert<Scale::TCB>(from);
                return convert<Scale::TDB>(tcb);
            };
        };
};

int main() {

    return 0;
}