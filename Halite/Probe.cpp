#include "Probe.hpp"

Probe::Probe(int l0, int l1)
{
    pinLoc[0] = l0;
    pinLoc[1] = l1;
}

void Probe::stamp(MNASystem & m)
{
    // vp + vn - vd = 0
    m.stamp(nets[2], nets[0], +1, 0, nullptr);
    m.stamp(nets[2], nets[1], -1, 0, nullptr);
    m.stamp(nets[2], nets[2], -1, 0, nullptr);

    m.nodes[nets[2]].name = "v:probe";
}