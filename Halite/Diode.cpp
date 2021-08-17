#include "Diode.hpp"

// l0 -->|-- l1 -- parameters default to approx 1N4148
Diode::Diode(int l0, int l1, double rs, double is, double n) : rs(rs)
{
    pinLoc[0] = l0;
    pinLoc[1] = l1;

    initJunctionPN(pn, is, n);

    // FIXME: move init to some restart routine?

    // initial condition v = 0
    linearizeJunctionPN(pn, 0);
}

bool Diode::newton(IMNASystem & m)
{
    return newtonJunctionPN(pn, m.getValue(nets[2]));
}

void Diode::stamp(IMNASystem & m)
{
    // Diode could be built with 3 extra nodes:
    //
    // |  .  .    .       . +1 | V+
    // |  .  .    .       . -1 | V-
    // |  .  .  grs    -grs -1 | v:D
    // |  .  . -grs grs+geq  . | v:pn = ieq
    // | -1 +1   +1       .  . | i:pn
    //
    // Here grs is the 1/rs series conductance.
    //
    // This gives us the junction voltage (v:pn) and
    // current (i:pn) and the composite voltage (v:D).
    //
    // The i:pn row is an ideal transformer connecting
    // the floating diode to the ground-referenced v:D
    // where we connect the series resistance to v:pn
    // that solves the diode equation with Newton.
    //
    // We can then add the 3rd row to the bottom 2 with
    // multipliers 1 and -rs = -1/grs and drop it:
    //
    // |  .  .   . +1 | V+
    // |  .  .   . -1 | V-
    // |  .  . geq -1 | v:pn = ieq
    // | -1 +1  +1 rs | i:pn
    //
    // Note that only the v:pn row here is non-linear.
    //
    // We could even do away without the separate row for
    // the current, which would lead to the following:
    //
    // | +grs -grs     -grs |
    // | -grs +grs     +grs |
    // | -grs +grs +grs+geq | = ieq
    //
    // In practice we keep the current row since it's
    // nice to have it as an output anyway.
    //
    m.stamp(nets[3], nets[0], -1, 0, nullptr);
    m.stamp(nets[3], nets[1], +1, 0, nullptr);
    m.stamp(nets[3], nets[2], +1, 0, nullptr);

    m.stamp(nets[0], nets[3], +1, 0, nullptr);
    m.stamp(nets[1], nets[3], -1, 0, nullptr);
    m.stamp(nets[2], nets[3], -1, 0, nullptr);

    m.stamp(nets[3], nets[3], rs, 0, nullptr);

    m.stamp(nets[2], nets[2], 0, 0, &pn.geq);
    m.stampRhs(nets[2], 0, 0, &pn.ieq);

    char buf[16];

    sprintf(buf, "v:D:%d,%d", pinLoc[0], pinLoc[1]);
    m.nodes[nets[2]].name = buf;

    sprintf(buf, "i:D:%d,%d", pinLoc[0], pinLoc[1]);
    m.nodes[nets[3]].name = buf;
    m.nodes[nets[3]].type = MNANodeInfo::tCurrent;
}
