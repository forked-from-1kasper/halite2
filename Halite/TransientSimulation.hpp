#pragma once

#include "Interface.hpp"
#include <vector>

#define EIGEN_NO_DEBUG
#include <Eigen/Dense>
#include <Eigen/SparseCore>

typedef Eigen::Triplet<double> Triplet;

// MNACell represents a single entry in the solution matrix,
// where we store constants and time-step dependent constants
// separately, plus collect pointers to dynamic variables.
//
// We track enough information here that we only need to stamp once.
struct MNACell : Triplet
{
    double g;       // simple values (eg. resistor conductance)
    double gtimed;  // time-scaled values (eg. capacitor conductance)
    double* gdyn;   // pointer to dynamic variable, added in once per solve

    double prelu;

    void clear();
    void initPos(size_t, size_t);
    void initLU(double stepScale);
    // restore matrix state and update dynamic values
    void updatePre();
};

typedef std::vector<MNACell>        MNACells;
typedef Eigen::SparseMatrix<double> SparseMatrixXd;

// Stores A and b for Ax = b, where x is the solution.
struct MNASystemEigen : public IMNASystem {
    ~MNASystemEigen();

    void setSize(int n) final;
    void stamp(int i, int j, double g, double gtime, double* gdyn) final;
    void stampRhs(int i, double g, double gtime, double* gdyn) final;

    double getValue(int) final;

    MNACells conn; MNACells vals;
    SparseMatrixXd* A; Eigen::VectorXd *b, *x;
};

typedef std::vector<IComponent*> ComponentList;

class TransientSimulation : ISimulation {
public:
    TransientSimulation(OnTickPtr onTick, int nodes);
    ~TransientSimulation();

    void addComponent(IComponent *) final;
    void buildSystem();
    void setTimeStep(double);
    void setMaxTime(double);
    void run() final;
    void tick(); void pause();

    const IMNASystem & getMNA() final;

    IExportList exports;
    void addExport(IExport*) final;

    void printHeaders();

protected:
    bool paused; double tMax;
    double tStep; int nets, states;
    ComponentList components;
    MNASystemEigen system;
    void update();
    bool newton();
    void initLU(double stepScale);
    void setStepScale(double stepScale);
    void updatePre();

    OnTickPtr onTick;
};
