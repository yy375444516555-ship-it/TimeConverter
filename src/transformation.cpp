    #include "../include/transformation.h"

    #include <Eigen/Dense>
    #include <Eigen/Geometry>

    using Eigen::Vector3d;


    Vector3d transformation::operator()(const Vector3d& r) const {
        return Q * r + dR; 
    }

    std::pair<Vector3d, Vector3d> transformation::operator()(const Vector3d& r, const Vector3d& v) const {
        Vector3d r_new = Q * r + dR;
        Vector3d v_new = Q*v + dV + w.cross(r_new);
        return {r_new, v_new};
    }

    transformation& transformation::operator*=(const transformation& other) {
        Vector3d w_rotated = other.Q * w;

        dR = other.Q * dR + other.dR;
        dV = other.Q * dV + other.dV - w_rotated.cross(other.dR);
        w = w_rotated + other.w;
        Q = other.Q * Q;

        return *this;
    }

    transformation transformation::operator*(const transformation& other) const {
        auto cpy = *this;
        cpy *= other;
        return cpy;
    }

    void transformation::inverse(){
        Q = Q.inverse();
        dR = Q * (-dR);
        dV = Q * (-dV) + (Q * w).cross(dR);   
        w = Q * (-w);
    }

    transformation transformation::inversed() const {
        auto cpy = *this;
        cpy.inverse();
        return cpy;
    }