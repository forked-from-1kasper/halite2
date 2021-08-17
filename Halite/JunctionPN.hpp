#pragma once

// POD-struct for PN-junction data, for diodes and BJTs
//
struct JunctionPN
{
    // variables
    double geq, ieq, veq;

    // parameters
    double is, nvt, rnvt, vcrit;
};

void initJunctionPN(JunctionPN & pn, double is, double n);
void linearizeJunctionPN(JunctionPN & pn, double v);
bool newtonJunctionPN(JunctionPN & pn, double v);
