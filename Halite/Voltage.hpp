#pragma once

#include "Component.hpp"

struct Voltage : Component<2, 1>
{
    double v;
    Voltage(double v, int l0, int l1);
    void stamp(MNASystem & m) final;
};