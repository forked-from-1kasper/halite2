#include "Components.hpp"
#include "Halite.hpp"

#include <vector>
#include <string>
#include <cstdio>
#include <cmath>

//
// General overview
// ----------------
//
// Circuits are built from nodes and Components, where nodes are
// simply positive integers (with 0 designating ground).
//
// Every Component has one or more pins connecting to the circuit
// nodes as well as zero or more internal nets.
//
// While we map pins directly to nets here, the separation would
// be useful if the solver implemented stuff like net-reordering.
//

double fnGen(double t)
{
    if (t < 0.0001) return 0;
    return (fmod(2000 * t, 1) > (0.5 + 0.4 * sin(2 * acos(-1) * 100 * t))) ? 0.25 : -0.25;
}

NetList* getTestCircuit(OnTickPtr onTick)
{
    /*

        BJT test circuit

        this is kinda good at catching problems :)

    */
    auto net = new NetList(onTick, 8);

    // node 1 is +12V
    net->addComponent(new Voltage(+12, 1, 0));

    // bias for base
    net->addComponent(new Resistor(100e3, 2, 1));
    net->addComponent(new Resistor(11e3, 2, 0));

    // input cap and function
    net->addComponent(new Capacitor(.1e-6, 2, 5));
    net->addComponent(new Resistor(1e3, 5, 6));
    net->addComponent(new Function(fnGen, 6, 0));

    // collector resistance
    net->addComponent(new Resistor(10e3, 1, 3));

    // emitter resistance
    net->addComponent(new Resistor(680, 4, 0));

    // bjt on b:2,c:3,e:4
    net->addComponent(new BJT(2,3,4));

    // output emitter follower (direct rail collector?)
    net->addComponent(new BJT(3, 1, 7));
    net->addComponent(new Resistor(100e3, 7, 0));

    net->addComponent(new Probe(7, 0));

    net->buildSystem();

    // get DC solution (optional)
    net->simulateTick();
    net->setTimeStep(1.0 / 44100.0);

    return net;
}
