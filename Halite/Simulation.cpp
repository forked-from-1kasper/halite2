#include "Constants.hpp"
#include "Halite.hpp"

#include <Eigen/SparseLU>
#include <iostream>

Simulation::Simulation(OnTickPtr func, int nodes) : onTick(func), nets(nodes), states(0)
{
    tMax = std::numeric_limits<double>::infinity();
    paused = true;
}

Simulation::~Simulation()
{
    for (const auto& c: components) delete c;
}

void Simulation::addComponent(IComponent * c)
{
    // this is a bit "temporary" for now
    c->setupNets(nets, states, c->getPinLocs());
    components.push_back(c);
}

void Simulation::buildSystem()
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

void Simulation::setTimeStep(double tStepSize)
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

void Simulation::setMaxTime(double maxTime)
{
    tMax = maxTime;
}

void Simulation::pause() { paused = true; }

void Simulation::run()
{
    paused = false;
    while (system.time <= tMax && !paused)
        tick();
}

void Simulation::tick()
{
    Eigen::SparseLU<SparseMatrixXd> solver;
    int iter; auto A = *system.A; auto b = *system.b;

    for (iter = 0; iter < maxIter; iter++) {
        // restore matrix state and add dynamic values
        updatePre();

        for (const auto& val: system.vals)
            b[val.row()] += val.value();

        A.setFromTriplets(system.conn.begin(), system.conn.end());

        solver.compute(A);
        if (solver.info() != Eigen::Success) printf("decomposition failed\n");

        *system.x = solver.solve(b);
        A.setZero(); b.setZero();

        if (solver.info() != Eigen::Success) printf("solving failed\n");

        if (newton()) break;
    }

    system.time += tStep; update();
    for (const auto& ex: exports) ex->extract(system);
    onTick(system);

    #ifdef VERBOSE
        printf(" %02.4f |", system.time);

        for (int i = 1; i < nets; ++i)
            printf("\t%+.4e", (*system.x)[i - 1] * system.nodes[i].scale);

        printf("\t %d iters\n", iter);
    #endif
}

void Simulation::printHeaders()
{
    printf("\n  time: |  ");
    for (int i = 1; i < nets; ++i)
        printf("%16s", system.nodes[i].name.c_str());
    printf("\n\n");
}

void Simulation::update()
{
    for (int i = 0; i < components.size(); ++i)
        components[i]->update(system);
}

// return true if we’re done
bool Simulation::newton()
{
    for (const auto & c: components)
        if (!c->newton(system))
            return false;

    return true;
}

void Simulation::initLU(double stepScale)
{
    for (auto & cell: system.conn) cell.initLU(stepScale);
    for (auto & cell: system.vals) cell.initLU(stepScale);
}

void Simulation::setStepScale(double stepScale)
{
    // initialize matrix for LU and save it to cache
    initLU(stepScale);
}

void Simulation::updatePre()
{
    for (auto & cell: system.conn) cell.updatePre();
    for (auto & cell: system.vals) cell.updatePre();
}

const MNASystem & Simulation::getMNA() { return system; }

void Simulation::addExport(IExport* er) {
    exports.push_back(er);
}