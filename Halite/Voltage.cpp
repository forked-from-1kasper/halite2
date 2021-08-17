#include "Voltage.hpp"

Voltage::Voltage(double v, int l0, int l1) : v(v)
{
    pinLoc[0] = l0;
    pinLoc[1] = l1;
}

void Voltage::stamp(IMNASystem & m)
{
    m.stamp(nets[0], nets[2], -1, 0, nullptr);
    m.stamp(nets[1], nets[2], +1, 0, nullptr);

    m.stamp(nets[2], nets[0], +1, 0, nullptr);
    m.stamp(nets[2], nets[1], -1, 0, nullptr);

    m.stampRhs(nets[2], v, 0, nullptr);

    char buf[40];
    sprintf(buf, "i:V(%+.2g):%d,%d", v, pinLoc[0], pinLoc[1]);
    m.nodes[nets[2]].name = buf;
    m.nodes[nets[2]].type = MNANodeInfo::tCurrent;
}
