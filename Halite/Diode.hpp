#pragma once

#include "Component.hpp"
#include "JunctionPN.hpp"

struct Diode : Component<2, 2>
{
    JunctionPN  pn;
    // should make these parameters
    double rs;

    Diode(int l0, int l1, double rs = 10., double is = 35e-12, double n = 1.24);
    bool newton(MNASystem & m) final;
    void stamp(MNASystem & m) final;
};
