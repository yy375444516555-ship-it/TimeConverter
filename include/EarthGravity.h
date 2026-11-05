#pragma once

#include "Frame.h"
#include "classTime.h"
#include "Satellite.h"

#include <Eigen/Dense>
#include <GeographicLib/GravityModel.hpp>

template<typename FrameConverterT>
class EarthGravity {
    const GeographicLib::GravityModel& gravityModel_;
    const FrameConverterT& frameConverter_;

    public:
        EarthGravity(GeographicLib::GravityModel& gravityModel, FrameConverterT& frameConverter)
        : gravityModel_(gravityModel), frameConverter_(frameConverter) {}

        Eigen::Vector3d operator()( const Eigen::Vector3d& pos_gcrs,
                                    const Eigen::Vector3d& velocity,
                                    const Time<Scale::TT>& time,
                                    const Satellite& sat) const{
            const auto GCRStoITRS = frameConverter_(time, Frame::ITRS, Frame::GCRS);

            const Eigen::Vector3d pos_itrs = GCRStoITRS(pos_gcrs);

            //ускорение в ITRS
            double ax = 0.0;
            double ay = 0.0;
            double az = 0.0;

            gravityModel_.V(pos_itrs.x(), pos_itrs.y(), pos_itrs.z(), ax, ay, az);

            const Eigen::Vector3d acceleration_itrs(ax, ay, az);
            const auto ITRStoGCRS = frameConverter_(time, Frame::GCRS, Frame::ITRS);

            const Eigen::Vector3d acceleration_gcrs = ITRStoGCRS(acceleration_itrs);

            return acceleration_gcrs;
        };
}


