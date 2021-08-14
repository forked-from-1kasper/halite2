#include "Constants.hpp"
#include "Halite.hpp"

NetList::NetList(int nodes) : nets(nodes), states(0)
{
}

NetList::~NetList()
{
    for (const auto& c: components) {
        delete c;
    }
}

void NetList::addComponent(IComponent * c)
{
    // this is a bit "temporary" for now
    c->setupNets(nets, states, c->getPinLocs());
    components.push_back(c);
}

void NetList::buildSystem()
{
    system.setSize(nets);
    for (int i = 0; i < components.size(); ++i)
        components[i]->stamp(system);

    #ifdef VERBOSE
        printf("Prepare for DC analysis..\n");
    #endif

    setStepScale(0);
    tStep = 0;
}

void NetList::dumpMatrix()
{
    std::vector<int> maxWidth(nets);

    for (int i = 0; i < nets; ++i) maxWidth[i] = 1;
    int nnMax = 1;

    for (int i = 0; i < nets; ++i)
    {
        nnMax = std::max(nnMax, (int) system.nodes[i].name.size());
        for (int j = 0; j < nets; ++j)
        {
            maxWidth[j] = std::max(maxWidth[j],
                (int)system.A[i][j].txt.size());
        }
    }


    char buf[1024];
    for (unsigned i = 0; i < nets; ++i)
    {
        int off = sprintf(buf, "%2d: | ", i);
        for (int j = 0; j < nets; ++j)
        {
            off += sprintf(buf+off,
                " %*s ", maxWidth[j],
                system.A[i][j].txt.size()
                ? system.A[i][j].txt.c_str()
                : ((system.A[i][j].lu==0) ? "." : "#"));
        }
        sprintf(buf+off, " | %-*s = %s",
            nnMax, system.nodes[i].name.c_str(),
            system.b[i].txt.size()
            ? system.b[i].txt.c_str() : (!i ? "ground" : "0"));

        puts(buf);
    }
}

void NetList::setTimeStep(double tStepSize)
{
    for (int i = 0; i < components.size(); ++i)
        components[i]->scaleTime(tStep / tStepSize);

    tStep = tStepSize;
    double stepScale = 1. / tStep;

    #ifdef VERBOSE
        printf("timeStep changed to %.2g (%.2g Hz)\n", tStep, stepScale);
    #endif

    setStepScale(stepScale);
}

void NetList::simulateTick()
{
    int iter;
    for (iter = 0; iter < maxIter; ++iter)
    {
        // restore matrix state and add dynamic values
        updatePre();
        luFactor();
        luForward();
        luSolve();

        if (newton()) break;
    }

    system.time += tStep;

    update();

    #ifdef VERBOSE
        printf(" %02.4f |", system.time);

        int fillPost = 0;
        for (int i = 1; i < nets; ++i)
        {
            printf("\t%+.4e", system.b[i].lu * system.nodes[i].scale);

            for (int j = 1; j < nets; ++j)
                if (system.A[i][j].lu != 0) ++fillPost;
        }

        printf("\t %d iters, LU density: %.1f%%\n",
            iter, 100 * fillPost / ((nets-1.f)*(nets-1.f)));
    #endif
}

void NetList::printHeaders()
{
    printf("\n  time: |  ");
    for (int i = 1; i < nets; ++i)
    {
        printf("%16s", system.nodes[i].name.c_str());
    }
    printf("\n\n");
}

void NetList::update()
{
    for (int i = 0; i < components.size(); ++i)
        components[i]->update(system);
}

// return true if we're done
bool NetList::newton()
{
    bool done = 1;
    for (int i = 0; i < components.size(); ++i)
    {
        done &= components[i]->newton(system);
    }
    return done;
}

void NetList::initLU(double stepScale)
{
    for (int i = 0; i < nets; ++i)
    {
        system.b[i].initLU(stepScale);
        for (int j = 0; j < nets; ++j)
        {
            system.A[i][j].initLU(stepScale);
        }
    }
}

void NetList::setStepScale(double stepScale)
{
    // initialize matrix for LU and save it to cache
    initLU(stepScale);

    int fill = 0;
    for (int i = 1; i < nets; ++i)
    {
        for (int j = 1; j < nets; ++j)
        {
            if (system.A[i][j].prelu != 0
             || system.A[i][j].gdyn.size()) ++fill;
        }
    }

    #ifdef VERBOSE
        printf("MNA density %.1f%%\n", 100 * fill / ((nets-1.)*(nets-1.)));
    #endif
}

void NetList::updatePre()
{
    for (int i = 0; i < nets; ++i)
    {
        system.b[i].updatePre();
        for (int j = 0; j < nets; ++j)
            system.A[i][j].updatePre();
    }
}

void NetList::luFactor()
{
    int p;
    for (p = 1; p < nets; ++p)
    {
        // FIND PIVOT
        {
            int pr = p;
            for (int r = p; r < nets; ++r)
            {
                if(fabs(system.A[r][p].lu)
                > fabs(system.A[pr][p].lu))
                {
                    pr = r;
                }
            }
            // swap if necessary
            if (pr != p)
            {
                std::swap(system.A[p], system.A[pr]);
                std::swap(system.b[p], system.b[pr]);
            }
            if (VERBOSE_LU)
            {
              printf("pivot %d (from %d): %+.2e\n",
                     p, pr, system.A[p][p].lu);
            }
        }
        if (0 == system.A[p][p].lu)
        {
            printf("Failed to find a pivot!!");
            return;
        }

        // take reciprocal for D entry
        system.A[p][p].lu = 1 / system.A[p][p].lu;

        // perform reduction on rows below
        for (int r = p+1; r < nets; ++r)
        {
            if (system.A[r][p].lu == 0) continue;

            system.A[r][p].lu *= system.A[p][p].lu;
            for (int c = p + 1; c < nets; ++c)
            {
                if (system.A[p][c].lu == 0) continue;
                system.A[r][c].lu -= system.A[p][c].lu * system.A[r][p].lu;
            }

        }
    }
}

// this does forward substitution for the solution vector
int NetList::luForward()
{
    int p;
    for (p = 1; p < nets; ++p)
    {
        // perform reduction on rows below
        for (int r = p+1; r < nets; ++r)
        {
            if (system.A[r][p].lu == 0) continue;
            if (system.b[p].lu == 0) continue;

            system.b[r].lu -= system.b[p].lu * system.A[r][p].lu;
        }
    }
    return p;
}

// solves nodes backwards from limit-1
// if solveAll is true, solves all the nodes
// otherwise if solveNoIter is true, solves until !wantUpdate
// if both flags are false, solves until !wantIter
int NetList::luSolve()
{
    for (int r = nets; --r;)
    {
        //printf("solve node %d\n", r);
        for (int s = r + 1; s < nets; ++s)
        {
            system.b[r].lu -= system.b[s].lu * system.A[r][s].lu;
        }

        system.b[r].lu *= system.A[r][r].lu;
    }
    return 1;
}
