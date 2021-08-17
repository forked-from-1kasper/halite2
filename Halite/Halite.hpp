#pragma once

#include <vector>
#include <string>
#include <cmath>

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

// this is for keeping track of node information
// for the purposes of more intelligent plotting
struct MNANodeInfo
{
    enum Type
    {
        tVoltage,
        tCurrent,

        tCount
    };

    Type    type;   // one auto-range per unit-type
    double  scale;  // scale factor (eg. charge to voltage)

    std::string name;   // node name for display
};

typedef std::vector<MNACell>        MNACells;
typedef Eigen::SparseMatrix<double> SparseMatrixXd;

// Stores A and b for Ax = b, where x is the solution.
struct MNASystem
{
    ~MNASystem();

    // node names - for output
    std::vector<MNANodeInfo> nodes;

    double time; MNACells conn; MNACells vals;
    SparseMatrixXd* A; Eigen::VectorXd *b, *x;

    void setSize(int n);
    void stamp(int i, int j, double g, double gtime, double* gdyn);
    void stampValue(int i, double g, double gtime, double* gdyn);

    double getValue(int);
};

struct IExport {
    virtual void extract(MNASystem & m) {};
};

template <class T>
struct Export : public IExport {
    virtual T value() { return value(); }
};

struct IComponent
{
    virtual ~IComponent() {}

    // return the number of pins for this component
    virtual int pinCount() = 0;

    // return a pointer to array of pin locations
    // NOTE: these will eventually be GUI locations to be unified
    virtual const int* getPinLocs() const = 0;

    // setup pins and calculate the size of the full netlist
    // the Component<> will handle this automatically
    //
    //  - netSize is the current size of the netlist
    //  - pins is an array of circuits nodes
    //
    virtual void setupNets(int & netSize, int & states, const int* pins) = 0;

    // stamp constants into the matrix
    virtual void stamp(MNASystem & m) = 0;

    // this is for allocating state variables
    virtual void setupStates(int & states) {}

    // update state variables, only tagged nodes
    // this is intended for fixed-time compatible
    // testing to make sure we can code-gen stuff
    virtual void update(MNASystem & m) {}

    // return true if we're done - will keep iterating
    // until all the components are happy
    virtual bool newton(MNASystem & m) { return true; }

    // time-step change, for caps to fix their state-variables
    virtual void scaleTime(double told_per_new) {}
};

typedef std::vector<IComponent*> ComponentList;
typedef std::vector<IExport*> IExportList;
typedef void (*OnTickPtr)(MNASystem & m);

class NetList {
public:
    NetList(OnTickPtr onTick, int nodes);
    ~NetList();

    void addComponent(IComponent *);
    void buildSystem();
    void setTimeStep(double);
    void setMaxTime(double);
    void simulateTick();
    void run(); void pause();
    void printHeaders();

    const MNASystem & getMNA();

    IExportList exports;
    void addExport(IExport*);

protected:
    bool paused; double tMax;
    double tStep; int nets, states;
    ComponentList components;
    MNASystem system;
    void update();
    bool newton();
    void initLU(double stepScale);
    void setStepScale(double stepScale);
    void updatePre();

    OnTickPtr onTick;
};

NetList* getTestCircuit(OnTickPtr);