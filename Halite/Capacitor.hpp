#pragma once

#include "Component.hpp"

struct Capacitor : Component<2, 1>
{
    double c; double stateVar; double voltage;
    Capacitor(double c, int l0, int l1);
    void stamp(IMNASystem & m) final;
    void update(IMNASystem & m) final;
    void scaleTime(double told_per_new) final;
};