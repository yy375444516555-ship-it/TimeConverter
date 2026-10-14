#pragma once

#include "classTime.h"
#include "celestial_body.h"

#include <calceph/calceph.h>
#include <Eigen/Dense>

#include <string>

class Ephemeris {
private:
    t_calcephbin* eph_ = nullptr;

    static int bodyID(celestial_body body);

public:

    double get_gm(celestial_body body) const;

    explicit Ephemeris(const std::string& path);

    ~Ephemeris();

    Ephemeris(const Ephemeris&) = delete;
    Ephemeris& operator=(const Ephemeris&) = delete;

    Eigen::Vector3d position(celestial_body body, const Time<Scale::TT>& time) const;

};