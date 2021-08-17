#include "Function.hpp"

Function::Function(FuncPtr fn, int l0, int l1) : fn(fn)
{
    pinLoc[0] = l0;
    pinLoc[1] = l1;
    v = fn(0);
}

void Function::stamp(IMNASystem & m)
{
    // this is identical to voltage source
    // except voltage is dynanic
    m.stamp(nets[0], nets[2], -1, 0, nullptr);
    m.stamp(nets[1], nets[2], +1, 0, nullptr);

    m.stamp(nets[2], nets[0], +1, 0, nullptr);
    m.stamp(nets[2], nets[1], -1, 0, nullptr);

    m.stampValue(nets[2], 0, 0, &v);

    char buf[16];
    sprintf(buf, "i:Vfn:%d,%d", pinLoc[0], pinLoc[1]);
    m.nodes[nets[2]].name = buf;
    m.nodes[nets[2]].type = MNANodeInfo::tCurrent;
}

void Function::update(IMNASystem & m)
{
    v = fn(m.time);
}