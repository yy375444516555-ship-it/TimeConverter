#include "../include/Ephemeris.h"

#include <sofa.h>

#include <stdexcept>

Ephemeris::Ephemeris(const std::string& path){
    eph_ = calceph_open(path.c_str());

        if (eph_ == nullptr) {
            throw std::runtime_error("Can not open ephemeris file");
        }
}

Ephemeris::~Ephemeris(){
    if (eph_ != nullptr){
            calceph_close(eph_);
        }
}

int Ephemeris::bodyID(celestial_body body){
    switch (body){
         case celestial_body::Sun:
            return NAIFID_SUN;
        case celestial_body::Mercury:
            return NAIFID_MERCURY_BARYCENTER;
        case celestial_body::Venus:
            return NAIFID_VENUS_BARYCENTER;
        case celestial_body::Earth:
            return NAIFID_EARTH;
        case celestial_body::Moon:
            return NAIFID_MOON;
        case celestial_body::Mars:
            return NAIFID_MARS_BARYCENTER;
        case celestial_body::Jupiter:
            return NAIFID_JUPITER_BARYCENTER;
        case celestial_body::Saturn:
            return NAIFID_SATURN_BARYCENTER;
        case celestial_body::Uranus:
            return NAIFID_URANUS_BARYCENTER;
        case celestial_body::Neptun:
            return NAIFID_NEPTUNE_BARYCENTER;
    }
}

Eigen::Vector3d Ephemeris::position(celestial_body body, const Time<Scale::TT>& time) const{

     //TT -> TDB
    double jd1 = time.getJdInt();
    double jd2 = time.getJdFrac();

    double dtr = iauDtdb(jd1, jd2, 0, 0, 0, 0);

    double tdb1, tdb2;

    int status1 = iauTttdb(jd1, jd2, dtr, &tdb1, &tdb2);
    if (status1 < 0)  throw std::runtime_error("Error TT->TDB");

    double pv[6] = {};

    int units = CALCEPH_UNIT_KM | CALCEPH_UNIT_SEC | CALCEPH_USE_NAIFID;

    int status = calceph_compute_unit(
        eph_,
        time.getJdInt(),
        time.getJdFrac(),
        Ephemeris::bodyID(body),
        NAIFID_EARTH,
        units,
        pv
    );

    if (status == 0) throw std::runtime_error("CALCEPH calculation failed");

    return Eigen::Vector3d(pv[0], pv[1], pv[2]) * 1000;
}

double Ephemeris::get_gm(celestial_body body) const {
    const char* name = nullptr;

    switch (body) {
        case celestial_body::Sun:
            name = "GMS";
            break;
        case celestial_body::Mercury:
            name = "GM1";
            break;
        case celestial_body::Venus:
            name = "GM2";
            break;
        case celestial_body::Mars:
            name = "GM4";
            break;
        case celestial_body::Jupiter:
            name = "GM5";
            break;
        case celestial_body::Saturn:
            name = "GM6";
            break;
        case celestial_body::Uranus:
            name = "GM7";
            break;
        case celestial_body::Neptun:
            name = "GM8";
            break;
        case celestial_body::Moon:
            name = "GMB";
            break;
        case celestial_body::Earth:
            throw std::invalid_argument("Earth is not a third body");
    }

    if (name == nullptr) {
        throw std::invalid_argument("Unknown celestial body");
    }

    double gm = 0.0;
    double au = 0.0;

    if (calceph_getconstant(eph_, name, &gm) == 0 ||
        calceph_getconstant(eph_, "AU", &au) == 0) {
        throw std::runtime_error("Cannot read GM");
    }

    if (body == celestial_body::Moon) {
        double emrat = 0.0;
        if (calceph_getconstant(eph_, "EMRAT", &emrat) == 0) {
            throw std::runtime_error("Cannot read EMRAT");
        }
        gm = gm / (1.0 + emrat);
    }

    au *= 1000.0;

    return gm * au * au * au / (86400.0 * 86400.0);
}