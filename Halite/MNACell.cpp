#include "Halite.hpp"

void MNACell::clear()
{
    g = 0;
    gtimed = 0;
    txt = "";
}

void MNACell::initLU(double stepScale)
{
    prelu = g + gtimed * stepScale;
}

void MNACell::updatePre()
{
    lu = prelu;
    for (int i = 0; i < gdyn.size(); ++i)
        lu += *(gdyn[i]);
}
