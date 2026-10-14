#pragma once

#include "celestial_body.h"
#include "classTime.h"
#include "Ephemeris.h"
#include "Satellite.h"

#include <Eigen/Dense>

using Eigen::Vector3d;

class Third_body_gravity{
    const Ephemeris& eph_;
    std::vector<celestial_body> bodies_;

    public:
    Third_body_gravity(const Ephemeris& eph, std::vector<celestial_body> bodies) : eph_(eph), bodies_(bodies){}

    Vector3d operator()(const Vector3d& position, 
                        const Vector3d& velocity,
                        const Time<Scale::TT>& time,
                        const Satellite& sat) const;

};


