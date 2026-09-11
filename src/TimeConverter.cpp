#include "../include/TimeConverter.h"

#include <stdexcept>

template<typename DutContainer, typename DatContainer>
TimeConverter<DutContainer, DatContainer>::TimeConverter(
    const DutContainer& dutContainer,
    const DatContainer& datContainer,
    const SomeTcbContainer& tcbContainer
)
    : dutContainer_(dutContainer),
      datContainer_(datContainer),
      tcbContainer_(tcbContainer) {}

template<typename DutContainer, typename DatContainer>
template<Scale To, Scale From>
Time<To> TimeConverter<DutContainer, DatContainer>::convert(
const Time<From>& from) const {
    if constexpr (To == From) {
        return Time<To>::fromJD(from.getJd());
    }
    else if constexpr (From == Scale::UTC && To == Scale::UT1) {
        const double dutDay = dutContainer_.dut(from.getMjd()) / 86400.0;
        return Time<To>(from.getJdInt(), from.getJdFrac() + dutDay);
    }
    else if constexpr (From == Scale::UT1 && To == Scale::UTC) {
        const double dutDay = dutContainer_.dut(from.getMjd()) / 86400.0;
        return Time<To>(from.getJdInt(), from.getJdFrac() - dutDay);
    }
    else if constexpr (From == Scale::TAI && To == Scale::TT) {
        return Time<To>(
            from.getJdInt(),
            from.getJdFrac() + 32.184 / 86400.0
        );
    }
    else if constexpr (From == Scale::TT && To == Scale::TAI) {
        return Time<To>(
            from.getJdInt(),
            from.getJdFrac() - 32.184 / 86400.0
        );
    }
    else if constexpr (From == Scale::TAI && To == Scale::UTC) {
        const double datDay = datContainer_.dat(from.getMjd()) / 86400.0;
        return Time<To>(from.getJdInt(), from.getJdFrac() - datDay);
    }
    else if constexpr (From == Scale::UTC && To == Scale::TAI) {
        const double datDay = datContainer_.dat(from.getMjd()) / 86400.0;
        return Time<To>(from.getJdInt(), from.getJdFrac() + datDay);
    }
    else if constexpr (From == Scale::TAI && To == Scale::UT1) {
        const auto utc = convert<Scale::UTC>(from);
        return convert<Scale::UT1>(utc);
    }
    else if constexpr (From == Scale::UT1 && To == Scale::TAI) {
        const auto utc = convert<Scale::UTC>(from);
        return convert<Scale::TAI>(utc);
    }
    else if constexpr (From == Scale::UTC && To == Scale::TT) {
        const auto tai = convert<Scale::TAI>(from);
        return convert<Scale::TT>(tai);
    }
    else if constexpr (From == Scale::TT && To == Scale::UTC) {
        const auto tai = convert<Scale::TAI>(from);
        return convert<Scale::UTC>(tai);
    }
    else if constexpr (From == Scale::UT1 && To == Scale::TT) {
        const auto utc = convert<Scale::UTC>(from);
        return convert<Scale::TT>(utc);
    }
    else if constexpr (From == Scale::TT && To == Scale::UT1) {
        const auto utc = convert<Scale::UTC>(from);
        return convert<Scale::UT1>(utc);
    }
    else if constexpr (From == Scale::TT && To == Scale::TCG) {
        constexpr double LG = 6.969290134e-10;
        constexpr double TT0 = 2443144.5003725;

        const double deltaJd = from.getJd() - TT0;
        const double tcgJdFrac = from.getJdFrac() + LG * deltaJd;

        return Time<To>(from.getJdInt(), tcgJdFrac);
    }
    else if constexpr (From == Scale::TCG && To == Scale::TCB) {
        const double dtcbDay = tcbContainer_.dtcb(from.getMjd()) / 86400.0;
        return Time<To>(from.getJdInt(), from.getJdFrac() + dtcbDay);
    }
    else if constexpr (From == Scale::TCB && To == Scale::TDB) {
        constexpr double LB = 1.550519768e-8;
        constexpr double T0_INT = 2443144.0;
        constexpr double T0_FRAC = 0.5003725;
        constexpr double TDB0 = -6.55e-5;

        const double deltaJd =
            (from.getJdInt() - T0_INT)
            + (from.getJdFrac() - T0_FRAC);

        const double correctionSec = -LB * deltaJd * 86400.0 + TDB0;
        const double tdbJdFrac =
            from.getJdFrac() + correctionSec / 86400.0;

        return Time<To>(from.getJdInt(), tdbJdFrac);
    }
    else if constexpr (From == Scale::TAI && To == Scale::TCG) {
        const auto tt = convert<Scale::TT>(from);
        return convert<Scale::TCG>(tt);
    }
    else if constexpr (From == Scale::UTC && To == Scale::TCG) {
        const auto tt = convert<Scale::TT>(from);
        return convert<Scale::TCG>(tt);
    }
    else if constexpr (From == Scale::UT1 && To == Scale::TCG) {
        const auto tt = convert<Scale::TT>(from);
        return convert<Scale::TCG>(tt);
    }
    else if constexpr (From == Scale::TT && To == Scale::TCB) {
        const auto tcg = convert<Scale::TCG>(from);
        return convert<Scale::TCB>(tcg);
    }
    else if constexpr (From == Scale::TAI && To == Scale::TCB) {
        const auto tcg = convert<Scale::TCG>(from);
        return convert<Scale::TCB>(tcg);
    }
    else if constexpr (From == Scale::UTC && To == Scale::TCB) {
        const auto tcg = convert<Scale::TCG>(from);
        return convert<Scale::TCB>(tcg);
    }
    else if constexpr (From == Scale::UT1 && To == Scale::TCB) {
        const auto tcg = convert<Scale::TCG>(from);
        return convert<Scale::TCB>(tcg);
    }
    else if constexpr (From == Scale::TCG && To == Scale::TDB) {
        const auto tcb = convert<Scale::TCB>(from);
        return convert<Scale::TDB>(tcb);
    }
    else if constexpr (From == Scale::TT && To == Scale::TDB) {
        const auto tcb = convert<Scale::TCB>(from);
        return convert<Scale::TDB>(tcb);
    }
    else if constexpr (From == Scale::TAI && To == Scale::TDB) {
        const auto tcb = convert<Scale::TCB>(from);
        return convert<Scale::TDB>(tcb);
    }
    else if constexpr (From == Scale::UTC && To == Scale::TDB) {
        const auto tcb = convert<Scale::TCB>(from);
        return convert<Scale::TDB>(tcb);
    }
    else if constexpr (From == Scale::UT1 && To == Scale::TDB) {
        const auto tcb = convert<Scale::TCB>(from);
        return convert<Scale::TDB>(tcb);
    }
    else {
        throw std::logic_error("This time-scale conversion is not implemented");
    }
}

using Converter = TimeConverter<SomeDutContainer, SomeDATContainer>;

template class TimeConverter<SomeDutContainer, SomeDATContainer>;

#define INSTANTIATE_CONVERT(TO, FROM) \
    template Time<TO> Converter::convert<TO, FROM>(const Time<FROM>&) const



INSTANTIATE_CONVERT(Scale::UTC, Scale::UTC);
INSTANTIATE_CONVERT(Scale::UT1, Scale::UT1);
INSTANTIATE_CONVERT(Scale::TAI, Scale::TAI);
INSTANTIATE_CONVERT(Scale::TT,  Scale::TT);
INSTANTIATE_CONVERT(Scale::TCG, Scale::TCG);
INSTANTIATE_CONVERT(Scale::TCB, Scale::TCB);
INSTANTIATE_CONVERT(Scale::TDB, Scale::TDB);


INSTANTIATE_CONVERT(Scale::UT1, Scale::UTC);
INSTANTIATE_CONVERT(Scale::UTC, Scale::UT1);
INSTANTIATE_CONVERT(Scale::TT,  Scale::TAI);
INSTANTIATE_CONVERT(Scale::TAI, Scale::TT);
INSTANTIATE_CONVERT(Scale::UTC, Scale::TAI);
INSTANTIATE_CONVERT(Scale::TAI, Scale::UTC);
INSTANTIATE_CONVERT(Scale::UT1, Scale::TAI);
INSTANTIATE_CONVERT(Scale::TAI, Scale::UT1);
INSTANTIATE_CONVERT(Scale::TT,  Scale::UTC);
INSTANTIATE_CONVERT(Scale::UTC, Scale::TT);
INSTANTIATE_CONVERT(Scale::TT,  Scale::UT1);
INSTANTIATE_CONVERT(Scale::UT1, Scale::TT);


INSTANTIATE_CONVERT(Scale::TCG, Scale::TT);
INSTANTIATE_CONVERT(Scale::TCB, Scale::TCG);
INSTANTIATE_CONVERT(Scale::TDB, Scale::TCB);
INSTANTIATE_CONVERT(Scale::TCG, Scale::TAI);
INSTANTIATE_CONVERT(Scale::TCG, Scale::UTC);
INSTANTIATE_CONVERT(Scale::TCG, Scale::UT1);
INSTANTIATE_CONVERT(Scale::TCB, Scale::TT);
INSTANTIATE_CONVERT(Scale::TCB, Scale::TAI);
INSTANTIATE_CONVERT(Scale::TCB, Scale::UTC);
INSTANTIATE_CONVERT(Scale::TCB, Scale::UT1);
INSTANTIATE_CONVERT(Scale::TDB, Scale::TCG);
INSTANTIATE_CONVERT(Scale::TDB, Scale::TT);
INSTANTIATE_CONVERT(Scale::TDB, Scale::TAI);
INSTANTIATE_CONVERT(Scale::TDB, Scale::UTC);
INSTANTIATE_CONVERT(Scale::TDB, Scale::UT1);

#undef INSTANTIATE_CONVERT
