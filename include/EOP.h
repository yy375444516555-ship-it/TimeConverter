#pragma once

#include <vector>
#include <tuple>
#include <utility>
#include <cstddef>

struct DailyEOP{
    double mjd;
    double dut;
    double xp;
    double yp;
};

class EOP {
    std::vector<DailyEOP> data_;
    double mjd0;

    std::pair<std::size_t, double> get_index(const double mjd) const;


public:

    //Выполняет интерполяцию dut
    double dut(double mjd) const;

    //Выполняет интерполяцию (xp, yp)
    std::tuple<double, double> pole(double mjd) const;


};

