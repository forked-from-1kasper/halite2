#pragma once

#include <vector>
#include <string>

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

struct IMNASystem {
    double time; std::vector<MNANodeInfo> nodes;

    virtual void setSize(int n) {};
    virtual void stamp(int i, int j, double g, double gtime, double* gdyn) {};
    virtual void stampValue(int i, double g, double gtime, double* gdyn) {};

    virtual double getValue(int) { return 0; };
};

struct IExport {
    virtual void extract(IMNASystem & m) {};
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
    virtual void stamp(IMNASystem & m) = 0;

    // this is for allocating state variables
    virtual void setupStates(int & states) {}

    // update state variables, only tagged nodes
    // this is intended for fixed-time compatible
    // testing to make sure we can code-gen stuff
    virtual void update(IMNASystem & m) {}

    // return true if we're done - will keep iterating
    // until all the components are happy
    virtual bool newton(IMNASystem & m) { return true; }

    // time-step change, for caps to fix their state-variables
    virtual void scaleTime(double told_per_new) {}
};

class ISimulation {
public:
    virtual void addComponent(IComponent *) {};
    virtual void run() {};
    virtual const IMNASystem & getMNA() { return getMNA(); };
    virtual void addExport(IExport*) {};
};
