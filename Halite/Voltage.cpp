#include "Voltage.hpp"

Voltage::Voltage(double v, int l0, int l1) : v(v)
{
    pinLoc[0] = l0;
    pinLoc[1] = l1;
}

void Voltage::stamp(MNASystem & m)
{
    m.stampStatic(-1, nets[0], nets[2], "-1");
    m.stampStatic(+1, nets[1], nets[2], "+1");

    m.stampStatic(+1, nets[2], nets[0], "+1");
    m.stampStatic(-1, nets[2], nets[1], "-1");

    char buf[16];
    sprintf(buf, "%+.2gV", v);
    m.b[nets[2]].g = v;
    m.b[nets[2]].txt = buf;

    sprintf(buf, "i:V(%+.2g):%d,%d", v, pinLoc[0], pinLoc[1]);
    m.nodes[nets[2]].name = buf;
    m.nodes[nets[2]].type = MNANodeInfo::tCurrent;
}
