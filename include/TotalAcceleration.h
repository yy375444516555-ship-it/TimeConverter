#pragma once

#include "classTime.h"
#include "Satellite.h"

#include <Eigen/Dense>

#include <tuple>

template<typename... F>
class TotalAcceleration{
    std::tuple<F...> f_;

public:
    explicit TotalAcceleration(const F&... forces) : f_(f...) {}

    Eigen::Vector3d operator()(const Eigen::Vector3d& position,
                                const Eigen::Vector3d& velocity,
                                const Time<Scale::TT>& time,
                                const Satellite& sat) const;

};

template<typename... F>
Eigen::Vector3d TotalAcceleration<F...>::operator()(const Eigen::Vector3d& position,
                                const Eigen::Vector3d& velocity,
                                const Time<Scale::TT>& time,
                                const Satellite& sat) const {

        const auto sum = [&](const auto&... force) -> Eigen::Vector3d {
            Eigen::Vector3d acceleration = Eigen::Vector3d::Zero();

            ((acceleration += force(position, velocity, time, sat)), ...);

            return acceleration;
        };

        return std::apply(sum, forces_);
    }
