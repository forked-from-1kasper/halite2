#include "Halite.hpp"

void MNASystem::setSize(int n)
{
    A.resize(n);
    b.resize(n);

    nodes.resize(n);

    for(unsigned i = 0; i < n; ++i)
    {
        b[i].clear();
        A[i].resize(n);

        char buf[16];
        sprintf(buf, "v%d", i);
        nodes[i].name = buf;
        nodes[i].type = MNANodeInfo::tVoltage;
        nodes[i].scale = 1;

        for (unsigned j = 0; j < n; ++j)
            A[i][j].clear();
    }

    time = 0;
}

void MNASystem::stampTimed(double g, int r, int c, const std::string & txt)
{
    A[r][c].gtimed += g;
    A[r][c].txt += txt;
}

void MNASystem::stampStatic(double g, int r, int c, const std::string & txt)
{
    A[r][c].g += g;
    A[r][c].txt += txt;
}

const MNASystem & NetList::getMNA() { return system; }
