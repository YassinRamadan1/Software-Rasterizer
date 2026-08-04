#pragma once
#include <iostream>
#include <vector>
#include "utility.h"

namespace clipper
{
    std::vector<Triangle> clipFarPlane(Triangle& triangle);

    std::vector<Triangle> clipNearPlane(Triangle& triangle);

    std::vector<Triangle> clip(Triangle& triangle);
};