#include "../include/TimeConverter.h"

#include <iomanip>
#include <array>
#include <iostream>
#include <GeographicLib/GravityModel.hpp> 

int main() {

    std::array<double, 3> position = {0, 0, 6372e3};

    GeographicLib::GravityModel gravityModel(
    "egm2008",
    R"(C:\Users\Yaraslau\Desktop\!!!\gravity)"
    );
    double gx, gy, gz;
    gravityModel.V(position[0], position[1], position[2], gx, gy, gz);
    std::cout << std::hypot(gx, gy, gz) << std::endl;

    return 0;
}
