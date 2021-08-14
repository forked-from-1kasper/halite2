#include "Constants.hpp"
#include "Resistor.hpp"

Resistor::Resistor(double r, int l0, int l1) : r(r)
{
    pinLoc[0] = l0;
    pinLoc[1] = l1;
}

void Resistor::stamp(MNASystem & m)
{
    char txt[16];
    txt[0] = 'R';
    formatUnitValue(txt+1, r, "");
    double g = 1. / r;
    m.stampStatic(+g, nets[0], nets[0], std::string("+") + txt);
    m.stampStatic(-g, nets[0], nets[1], std::string("-") + txt);
    m.stampStatic(-g, nets[1], nets[0], std::string("-") + txt);
    m.stampStatic(+g, nets[1], nets[1], std::string("+") + txt);
}
