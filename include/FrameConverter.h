#pragma once

#include "Frame.h"
#include "TimeConverter.h"
#include "transformation.h"

#include <sofa.h>
#include <Eigen/Dense>
#include <Eigen/Geometry>

using Eigen::Vector3d;
using Eigen::Quaterniond;

template<typename EOP, typename TimeConverter>
class FrameConverter {
    EOP eop_;
    TimeConverter tc_;
    
    public:
    FrameConverter(const EOP& eop, const TimeConverter& tc) : eop_(eop), tc_(tc) {}

    const EOP& eop() const {return eop_;}

    template<Scale sc>
    transformation operator()(const Time<sc>& t, Frame to, Frame from) const;

};

template<typename EOP, typename TimeConverter>
template<Scale sc>
transformation FrameConverter<EOP, TimeConverter>::operator()(const Time<sc>& t, Frame to, Frame from) const {
    if constexpr (to == Frame::ITRS && from == Frame::GCRS) {

    }
    else if constexpr (to == Frame::GCRS && from == Frame::ITRS){
        double rc2t[3][3];
        Time<TAI> tai = toTAI(t);
        Time<UT1> ut1 = fromTAI<UT1>(tai);
        transformation tr;

        rc2t = iauC2t06a(tai.getJdInt(), tai.getJdFrac(), ut1.getJdInt(), ut1.getJdFrac(), rc2t);

        tr.dR = Zero();
        tr.dV = Zero();
        tr.Q = Quaterniond(rc2t);
        tr.Q.normalize();
        
    }
}
