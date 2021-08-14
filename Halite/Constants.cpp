#include "Constants.hpp"
#include <cstdio>
#include <cmath>

void formatUnitValue(char * buf, double v, const char * unit)
{
    int suff = unitValueOffset + std::lround(std::floor(std::log(v) / std::log(10.))) / 3;

    if (v < 1) suff -= 1;

    if (suff < 0) suff = 0;
    if (suff > unitValueMax) suff = unitValueMax;

    double vr = v / std::pow(10.0, 3 * double(suff - unitValueOffset));

    sprintf(buf, "%.0f%s%s", vr, unitValueSuffixes[suff], unit);
}
