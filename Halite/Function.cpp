#include "Function.hpp"

Function::Function(FuncPtr fn, int l0, int l1) : fn(fn)
{
    pinLoc[0] = l0;
    pinLoc[1] = l1;
    v = fn(0);
}

void Function::stamp(MNASystem & m)
{
    // this is identical to voltage source
    // except voltage is dynanic
    m.stampStatic(-1, nets[0], nets[2], "-1");
    m.stampStatic(+1, nets[1], nets[2], "+1");

    m.stampStatic(+1, nets[2], nets[0], "+1");
    m.stampStatic(-1, nets[2], nets[1], "-1");

    char buf[16];
    m.b[nets[2]].gdyn.push_back(&v);
    sprintf(buf, "Vfn:%d,%d", pinLoc[0], pinLoc[1]);
    m.b[nets[2]].txt = buf;

    sprintf(buf, "i:Vfn:%d,%d", pinLoc[0], pinLoc[1]);
    m.nodes[nets[2]].name = buf;
    m.nodes[nets[2]].type = MNANodeInfo::tCurrent;
}

void Function::update(MNASystem & m)
{
    v = fn(m.time);
}