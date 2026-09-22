#pragma once

#include "classTime.h"
#include "EOP.h"

#include <sofa.h>
#include <stdexcept>

template<typename EOP>
class TimeConverter {
private:
    const EOP& eop_;

    Time<Scale::TAI> toTAI(const Time<Scale::TAI>& time) const; 
    Time<Scale::TAI> toTAI(const Time<Scale::UTC>& time) const; 
    Time<Scale::TAI> toTAI(const Time<Scale::UT1>& time) const;
    Time<Scale::TAI> toTAI(const Time<Scale::TT>& time) const;
    Time<Scale::TAI> toTAI(const Time<Scale::TCG>& time) const;
    Time<Scale::TAI> toTAI(const Time<Scale::TDB>& time) const;
    Time<Scale::TAI> toTAI(const Time<Scale::TCB>& time) const;

    template<Scale To>
    Time<To> fromTAI(const Time<Scale::TAI>& time) const;
    
public:
    TimeConverter(const EOP& eop) : eop_(eop) {}

    template<Scale To, Scale From>
    Time<To> convert(const Time<From>& from) const {
        auto tai = toTAI(from);
        return fromTAI<To>(tai);
    }
};

template <typename EOP>
Time<Scale::TAI> TimeConverter<EOP>::toTAI(const Time<Scale::TAI>& time) const {
    return time;
}

//UTC->TAI
template <typename EOP>
Time<Scale::TAI> TimeConverter<EOP>::toTAI(const Time<Scale::UTC>& time) const {
    double tai1;
    double tai2;

    int status = iauUtctai(time.getJdInt(), time.getJdFrac(), &tai1, &tai2);
    if (status < 0)  throw std::runtime_error("iauUtctai: unacceptable date");

    return Time<Scale::TAI>(tai1, tai2);
}
//UT1->TAI
template <typename EOP>
Time<Scale::TAI> TimeConverter<EOP>::toTAI(const Time<Scale::UT1>& time) const {
    double utc1;
    double utc2;
    
    double dut = eop_.dut(time.getMjd());
    int status = iauUt1utc(time.getJdInt(), time.getJdFrac(), dut, &utc1, &utc2);

    if (status < 0)  throw std::runtime_error("iauUt1utc: unacceptable date");

    const Time<Scale::UTC> utc(utc1, utc2);

    return toTAI(utc);
}
//TT->TAI
template <typename EOP>
Time<Scale::TAI> TimeConverter<EOP>::toTAI(const Time<Scale::TT>& time) const {
    double tai1;
    double tai2;

    int status = iauTttai(time.getJdInt(), time.getJdFrac(), &tai1, &tai2);

    if (status < 0)  throw std::runtime_error("iauTttai: unacceptable date");

    return Time<Scale::TAI>(tai1, tai2);
}

//TCG->TAI
template <typename EOP>
Time<Scale::TAI> TimeConverter<EOP>::toTAI(const Time<Scale::TCG>& time) const {
    double tt1;
    double tt2;

    int status = iauTcgtt(time.getJdInt(), time.getJdFrac(), &tt1, &tt2);
    if (status < 0)  throw std::runtime_error("iauTcgtt: unacceptable date");
    const Time<Scale::TT> tt(tt1, tt2);

    return toTAI(tt);
}

//TDB->TAI
template <typename EOP>
Time<Scale::TAI> TimeConverter<EOP>::toTAI(const Time<Scale::TDB>& time) const {
    double tt1;
    double tt2;
    double dtr = iauDtdb(time.getJdInt(), time.getJdFrac(), 0.0, 0.0, 0.0, 0.0);
    int status = iauTdbtt(time.getJdInt(), time.getJdFrac(), dtr , &tt1, &tt2);
    if (status < 0)  throw std::runtime_error("iauTdbtt: unacceptable date");
     
    const Time<Scale::TT> tt(tt1, tt2);
    return toTAI(tt);
}

//TCB->TAI
template <typename EOP>
Time<Scale::TAI> TimeConverter<EOP>::toTAI(const Time<Scale::TCB>& time) const {
    double tdb1;
    double tdb2;

    int status = iauTcbtdb(time.getJdInt(), time.getJdFrac(), &tdb1, &tdb2);
    if (status < 0) throw std::runtime_error("iauTcbtdb: unacceptable date");
    const Time<Scale::TDB> tdb(tdb1, tdb2);

    return toTAI(tdb);
}

template <typename EOP>
template <Scale To>
Time<To> TimeConverter<EOP>::fromTAI(const Time<Scale::TAI>& time) const {
    if constexpr (To == Scale::TAI) return time;

    else if constexpr(To == Scale::UTC){
        double utc1;
        double utc2;

        int status = iauTaiutc(time.getJdInt(), time.getJdFrac(), &utc1, &utc2);
        if (status < 0)  throw std::runtime_error("iauTaiutc: unacceptable date");

        return Time<Scale::UTC>(utc1, utc2);
    }

    else if constexpr (To == Scale::UT1){
        double ut1;
        double ut2;
        
        const Time<Scale::UTC> utc = fromTAI<Scale::UTC>(time);
        double dut = eop_.dut(utc.getMjd());

        int status = iauUtcut1(utc.getJdInt(), utc.getJdFrac(), dut, &ut1, &ut2);
        if (status < 0)  throw std::runtime_error("iauUtcut1: unacceptable date");

        return Time<Scale::UT1>(ut1, ut2);
    }

    else if constexpr (To == Scale::TT){
        double tt1;
        double tt2;

        int status = iauTaitt(time.getJdInt(), time.getJdFrac(), &tt1, &tt2);
        if (status < 0)  throw std::runtime_error("iauTaitt: unacceptable date");

        return Time<Scale::TT>(tt1, tt2);
    }

    else if constexpr (To == Scale::TCG){
        double tcg1;
        double tcg2;

        const Time<Scale::TT> tt = fromTAI<Scale::TT>(time);
        int status = iauTttcg(tt.getJdInt(), tt.getJdFrac(), &tcg1, &tcg2);
        if (status < 0)  throw std::runtime_error("iauTttcg: unacceptable date");

        return Time<Scale::TCG>(tcg1, tcg2);
    }
    
    else if constexpr (To == Scale::TDB){
        double tdb1;
        double tdb2;
        const Time<Scale::TT> tt = fromTAI<Scale::TT>(time);
        double dtr = iauDtdb(tt.getJdInt(), tt.getJdFrac(), 0.0, 0.0, 0.0, 0.0);
        int status = iauTttdb(tt.getJdInt(), tt.getJdFrac(), dtr , &tdb1,  &tdb2);
        if (status < 0)  throw std::runtime_error("iauTttdb: unacceptable date");
        
        return Time<Scale::TDB>(tdb1, tdb2);
    }

    else if constexpr (To == Scale::TCB){
        double tcb1;
        double tcb2;

        const Time<Scale::TDB> tdb = fromTAI<Scale::TDB>(time);

        int status = iauTdbtcb(tdb.getJdInt(), tdb.getJdFrac(), &tcb1, &tcb2);
        if (status < 0) throw std::runtime_error("iauTdbtcb: unacceptable date");

        return Time<Scale::TCB>(tcb1, tcb2);
    }
}
