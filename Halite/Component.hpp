#pragma once

#include "Interface.hpp"

template <int nPins = 0, int nInternalNets = 0>
struct Component : IComponent
{
    static const int nNets = nPins + nInternalNets;

    int pinLoc[nPins];
    int nets[nNets];

    int pinCount() final { return nPins; }

    const int* getPinLocs() const final { return pinLoc; }

    void setupNets(int & netSize, int & states, const int* pins) final
    {
        for (int i = 0; i < nPins; ++i)
            nets[i] = pins[i];

        for (int i = 0; i < nInternalNets; ++i)
            nets[nPins + i] = netSize++;

        setupStates(states);
    }
};
