#include "BJT.hpp"

BJT::BJT(int b, int c, int e, bool pnp) : pnp(pnp)
{
    pinLoc[0] = b;
    pinLoc[1] = c;
    pinLoc[2] = e;

    // this attempts a 2n3904-style small-signal
    // transistor, although the values are a bit
    // arbitrarily set to “something reasonable”

    // forward and reverse beta
    double bf = 200;
    double br = 20;

    // forward and reverse alpha
    af = bf / (1 + bf);
    ar = br / (1 + br);

    // these are just rb+re and rb+rc
    // this is not necessarily the best way to
    // do anything, but having junction series
    // resistances helps handle degenerate cases
    rsbc = 5.8376+0.0001;
    rsbe = 5.8376+2.65711;

    //
    // the basic rule is that:
    //  af * ise = ar * isc = is
    //
    // FIXME: with non-equal ideality factors
    // we can get non-sensical results, why?
    //
    double is = 6.734e-15;
    double n = 1.24;
    initJunctionPN(pnE, is / af, n);
    initJunctionPN(pnC, is / ar, n);

    linearizeJunctionPN(pnE, 0);
    linearizeJunctionPN(pnC, 0);
}

bool BJT::newton(MNASystem & m)
{
    return newtonJunctionPN(pnC, m.b[nets[3]].lu)
         & newtonJunctionPN(pnE, m.b[nets[4]].lu);
}


void BJT::stamp(MNASystem & m)
{
    // The basic idea here is the same as with diodes
    // except we do it once for each junction.
    //
    // With the transfer currents sourced from the
    // diode currents, NPN then looks like this:
    //
    // 0 |  .  .  .  .  . 1-ar 1-af | vB
    // 1 |  .  .  .  .  .   -1  +af | vC
    // 2 |  .  .  .  .  .  +ar   -1 | vE
    // 3 |  .  .  . gc  .   -1    . | v:Qbc  = ic
    // 4 |  .  .  .  . ge    .   -1 | v:Qbe  = ie
    // 5 | -1 +1  . +1  . rsbc    . | i:Qbc
    // 6 | -1  . +1  . +1    . rsbe | i:Qbe
    //     ------------------------
    //      0  1  2  3  4    5    6
    //
    // For PNP version, we simply flip the junctions
    // by changing signs of (3,5),(5,3) and (4,6),(6,4).
    //
    // Also just like diodes, we have junction series
    // resistances, rather than terminal resistances.
    //
    // This works just as well, but should be kept
    // in mind when fitting particular transistors.
    //

    // diode currents to external base
    m.stampStatic(1-ar, nets[0], nets[5], "1-ar");
    m.stampStatic(1-af, nets[0], nets[6], "1-af");

    // diode currents to external collector and emitter
    m.stampStatic(-1, nets[1], nets[5], "-1");
    m.stampStatic(-1, nets[2], nets[6], "-1");

    // series resistances
    m.stampStatic(rsbc, nets[5], nets[5], "rsbc");
    m.stampStatic(rsbe, nets[6], nets[6], "rsbe");

    // current - junction connections
    // for the PNP case we flip the signs of these
    // to flip the diode junctions wrt. the above
    if(pnp)
    {
        m.stampStatic(-1, nets[5], nets[3], "-1");
        m.stampStatic(+1, nets[3], nets[5], "+1");

        m.stampStatic(-1, nets[6], nets[4], "-1");
        m.stampStatic(+1, nets[4], nets[6], "+1");

    }
    else
    {
        m.stampStatic(+1, nets[5], nets[3], "+1");
        m.stampStatic(-1, nets[3], nets[5], "-1");

        m.stampStatic(+1, nets[6], nets[4], "+1");
        m.stampStatic(-1, nets[4], nets[6], "-1");
    }

    // external voltages to collector current
    m.stampStatic(-1, nets[5], nets[0], "-1");
    m.stampStatic(+1, nets[5], nets[1], "+1");

    // external voltages to emitter current
    m.stampStatic(-1, nets[6], nets[0], "-1");
    m.stampStatic(+1, nets[6], nets[2], "+1");

    // source transfer currents to external pins
    m.stampStatic(+ar, nets[2], nets[5], "+ar");
    m.stampStatic(+af, nets[1], nets[6], "+af");

    char buf[16];

    // dynamic variables
    m.A[nets[3]][nets[3]].gdyn.push_back(&pnC.geq);
    m.A[nets[3]][nets[3]].txt = "gm:Qbc";
    m.b[nets[3]].gdyn.push_back(&pnC.ieq);
    sprintf(buf, "i0:Q:%d,%d,%d:cb", pinLoc[0], pinLoc[1], pinLoc[2]);
    m.b[nets[3]].txt = buf;

    m.A[nets[4]][nets[4]].gdyn.push_back(&pnE.geq);
    m.A[nets[4]][nets[4]].txt = "gm:Qbe";
    m.b[nets[4]].gdyn.push_back(&pnE.ieq);
    sprintf(buf, "i0:Q:%d,%d,%d:eb", pinLoc[0], pinLoc[1], pinLoc[2]);
    m.b[nets[4]].txt = buf;

    sprintf(buf, "v:Q:%d,%d,%d:%s",
        pinLoc[0], pinLoc[1], pinLoc[2], pnp ? "cb" : "bc");
    m.nodes[nets[3]].name = buf;

    sprintf(buf, "v:Q:%d,%d,%d:%s",
        pinLoc[0], pinLoc[1], pinLoc[2], pnp ? "eb" : "be");
    m.nodes[nets[4]].name = buf;

    sprintf(buf, "i:Q:%d,%d,%d:bc", pinLoc[0], pinLoc[1], pinLoc[2]);
    m.nodes[nets[5]].name = buf;
    m.nodes[nets[5]].type = MNANodeInfo::tCurrent;
    m.nodes[nets[5]].scale = 1 - ar;

    sprintf(buf, "i:Q:%d,%d,%d:be", pinLoc[0], pinLoc[1], pinLoc[2]);
    m.nodes[nets[6]].name = buf;
    m.nodes[nets[6]].type = MNANodeInfo::tCurrent;
    m.nodes[nets[6]].scale = 1 - af;
}
