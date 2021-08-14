#pragma once

#include "Component.hpp"

typedef double (*FuncPtr)(double t);

// function voltage generator
struct Function : Component<2,1>
{
    FuncPtr fn; double v;
    Function(FuncPtr fn, int l0, int l1);
    void stamp(MNASystem & m) final;
    void update(MNASystem & m) final;
};