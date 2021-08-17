#include "Constants.hpp"
#include "Resistor.hpp"

Resistor::Resistor(double r, int l0, int l1) : r(r)
{
    pinLoc[0] = l0;
    pinLoc[1] = l1;
}

void Resistor::stamp(MNASystem & m)
{
    double g = 1. / r;

    m.stamp(nets[0], nets[0], +g, 0, nullptr);
    m.stamp(nets[0], nets[1], -g, 0, nullptr);
    m.stamp(nets[1], nets[0], -g, 0, nullptr);
    m.stamp(nets[1], nets[1], +g, 0, nullptr);
}
