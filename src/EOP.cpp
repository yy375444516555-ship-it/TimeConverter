#include "../include/EOP.h"

#include <cmath>

std::pair<std::size_t, double> EOP::get_index(const double mjd) const{
    const std::size_t d = std::floor(mjd - mjd0); 
    const double frac = mjd - mjd0 - d;
    return {d, frac};
}

double EOP::dut(const double mjd) const {
    
    const auto [d, frac] = get_index(mjd);
    if (d + 1 >= data_.size() || mjd < mjd0){
        return 0;
    };
    const double shift = 
        (data_[d+1].dut > data_[d].dut + 0.7 ? -1 :
        (data_[d+1].dut < data_[d].dut - 0.7 ? 1 : 0));

    return std::lerp(data_[d].dut,  data_[d+1].dut + shift, frac);
};

std::tuple<double, double> EOP::pole(double mjd) const {
    //заглушка
    return {0.0, 0.0};
}
