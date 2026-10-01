#pragma once

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include <utility>


struct transformation {
    Eigen::Vector3d dR = Eigen::Vector3d::Zero();
    Eigen::Vector3d dV = Eigen::Vector3d::Zero();
    Eigen::Quaterniond Q = Eigen::Quaterniond::Identity();
    Eigen::Vector3d w = Eigen::Vector3d::Zero();
    
    Eigen::Vector3d operator()(const Eigen::Vector3d& r) const;
        
    std::pair<Eigen::Vector3d, Eigen::Vector3d> operator()(const Eigen::Vector3d& r, const Eigen::Vector3d& v) const;

    transformation& operator*=(const transformation& other);

    transformation operator*(const transformation& other) const;

    void inverse();

    transformation inversed() const;

};