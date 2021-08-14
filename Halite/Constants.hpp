#pragma once

// gMin for diodes etc..
constexpr double gMin = 1e-12;

// voltage tolerance
constexpr double vTolerance = 5e-5;

// thermal voltage for diodes/transistors
constexpr double vThermal = 0.026;

constexpr unsigned maxIter = 200;

constexpr bool VERBOSE_LU = false;

constexpr int unitValueOffset = 4;
constexpr int unitValueMax = 8;
constexpr const char* unitValueSuffixes[] = {
    "p", "n", "u", "m", "", "k", "M", "G"
};

void formatUnitValue(char * buf, double v, const char * unit);