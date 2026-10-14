#include "../include/ThirdBodyGravity.h"

#include <vector>
#include <cmath>

#include <Eigen/Dense>

using Eigen::Vector3d;

Vector3d Third_body_gravity::operator()(const Vector3d& position, 
                        const Vector3d& velocity,
                        const Time<Scale::TT>& time,
                        const Satellite& sat) const {

    Vector3d sum = Vector3d::Zero();
    for (const auto b: bodies_) {
        const Vector3d pos = eph_.position(b, time);
        const Vector3d r = pos - position;
        const double gm = eph_.get_gm(b);
        const Vector3d acceleration = gm * ( r / std::pow(r.norm(), 3) - pos / std::pow(pos.norm(), 3));
        sum += acceleration;
    }
    return sum;
};