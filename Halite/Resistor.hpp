#pragma once

#include "Component.hpp"

struct Resistor : Component<2> {
    double r;
    Resistor(double r, int l0, int l1);
    void stamp(IMNASystem & m) final;
};
