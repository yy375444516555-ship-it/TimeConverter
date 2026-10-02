#pragma once

#include "Scale.h"
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
    if (to == Frame::ITRS && from == Frame::GCRS) {
        double rc2t[3][3], rc2t_minus[3][3], rc2t_plus[3][3];
        double dt = 1.0;
        auto tt = tc_.template convert<Scale::TT>(t);
        auto ut1 = tc_.template convert<Scale::UT1>(t);
        transformation tr;

        auto utc = tc_.template convert<Scale::UTC>(t);
        auto [xp, yp] = eop_.pole(utc.getMjd());

        iauC2t06a(tt.getJdInt(), tt.getJdFrac(), ut1.getJdInt(), ut1.getJdFrac(), xp, yp, rc2t);
        iauC2t06a(tt.getJdInt(), tt.getJdFrac() - (dt/86400.0), ut1.getJdInt(), ut1.getJdFrac() - (dt/86400.0), xp, yp, rc2t_minus);
        iauC2t06a(tt.getJdInt(), tt.getJdFrac() + (dt/86400.0), ut1.getJdInt(), ut1.getJdFrac() + (dt/86400.0), xp, yp, rc2t_plus);

        Eigen::Matrix3d R;
        Eigen::Matrix3d R_minus;
        Eigen::Matrix3d R_plus;

        for (int i = 0; i < 3; ++i){
            for(int j = 0; j < 3; ++j){
                R_minus(i, j) = rc2t_minus[i][j];
                R_plus(i, j) = rc2t_plus[i][j];
                R(i, j) = rc2t[i][j];
            }
        }

        Eigen::Matrix3d R_dif = (R_plus - R_minus) / (2 * dt);

        tr.Q = Quaterniond(R);
        tr.Q.normalize();
        Eigen::Matrix3d w_matrix = R_dif * R.transpose();
        tr.w.x() = (w_matrix(2, 1) - w_matrix(1, 2)) / 2.0;
        tr.w.y() = (w_matrix(0, 2) - w_matrix(2, 0)) / 2.0;
        tr.w.z() = (w_matrix(1, 0) - w_matrix(0, 1)) / 2.0;

        return tr;
    }
    else if (to == Frame::GCRS && from == Frame::ITRS){
        auto tr = (*this)(t, Frame::ITRS, Frame::GCRS);
        return tr.inversed();
    }
}
