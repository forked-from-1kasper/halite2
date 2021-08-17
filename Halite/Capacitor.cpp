#include "Capacitor.hpp"
#include "Constants.hpp"

Capacitor::Capacitor(double c, int l0, int l1) : c(c)
{
    pinLoc[0] = l0;
    pinLoc[1] = l1;

    stateVar = 0;
    voltage = 0;
}

void Capacitor::stamp(MNASystem & m)
{
    char buf[16];
    formatUnitValue(buf, c, "F");

    // we can use a trick here, to get the capacitor to
    // work on it’s own line with direct trapezoidal:
    //
    // | -g*t  +g*t  +t | v+
    // | +g*t  -g*t  -t | v-
    // | +2*g  -2*g  -1 | state
    //
    // the logic with this is that for constant timestep:
    //
    //  i1 = g*v1 - s0   , s0 = g*v0 + i0
    //  s1 = 2*g*v1 - s0 <-> s0 = 2*g*v1 - s1
    //
    // then if we substitute back:
    //  i1 = g*v1 - (2*g*v1 - s1)
    //     = s1 - g*v1
    //
    // this way we just need to copy the new state to the
    // next timestep and there’s no actual integration needed
    //
    // the “half time-step” error here means that our state
    // is 2*c*v - i/t but we fix this for display in update
    // and correct the current-part on time-step changes

    // trapezoidal needs another factor of two for the g
    // since c*(v1 - v0) = (i1 + i0)/(2*t), where t = 1/T
    double g = 2 * c;

    m.stamp(nets[0], nets[2], 0, +1, nullptr);
    m.stamp(nets[1], nets[2], 0, -1, nullptr);

    m.stamp(nets[0], nets[0], 0, -g, nullptr);
    m.stamp(nets[0], nets[1], 0, +g, nullptr);
    m.stamp(nets[1], nets[0], 0, +g, nullptr);
    m.stamp(nets[1], nets[1], 0, -g, nullptr);

    m.stamp(nets[2], nets[0], +2*g, 0, nullptr);
    m.stamp(nets[2], nets[1], -2*g, 0, nullptr);

    m.stamp(nets[2], nets[2], -1, 0, nullptr);

    // see the comment about v:C[%d] below
    m.stampValue(nets[2], 0, 0, &stateVar);

    // this isn't quite right as state stores 2*c*v - i/t
    // however, we'll fix this in updateFull() for display
    sprintf(buf, "v:C:%d,%d", pinLoc[0], pinLoc[1]);
    m.nodes[nets[2]].name = buf;
    m.nodes[nets[2]].scale = 1 / c;
}

void Capacitor::update(MNASystem & m)
{
    stateVar = m.getValue(nets[2]);

    // solve legit voltage from the pins
    voltage = m.getValue(nets[0]) - m.getValue(nets[1]);
}

void Capacitor::scaleTime(double told_per_new)
{
    // the state is 2*c*voltage - i/t0
    // so we subtract out the voltage, scale current
    // and then add the voltage back to get new state
    //
    // note that this also works if the old rate is infinite
    // (ie. t0=0) when going from DC analysis to transient
    //
    double qq = 2 * c * voltage;
    stateVar = qq + (stateVar - qq) * told_per_new;
}
