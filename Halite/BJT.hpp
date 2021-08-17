#pragma once

#include "Component.hpp"
#include "JunctionPN.hpp"

struct BJT : Component<3, 4>
{
    // emitter and collector junctions
    JunctionPN pnC, pnE;

    // forward and reverse alpha
    double af, ar, rsbc, rsbe;

    bool pnp;

    BJT(int b, int c, int e, bool pnp = false);
    bool newton(IMNASystem & m) final;
    void stamp(IMNASystem & m) final;
};