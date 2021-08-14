#pragma once

#include "Component.hpp"

// probe a differential voltage
// also forces this voltage to actually get solved :)
struct Probe : Component<2, 1>
{
    Probe(int l0, int l1);
    void stamp(MNASystem & m) final;

    //void update(MNASystem & m);
    // we could do output here :)
};