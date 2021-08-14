#include <Halite/Halite.hpp>

int main()
{
    auto net = setupCircuit();

    #ifdef VERBOSE
        net->printHeaders();
    #endif

    for(int i = 0; i < 16; ++i)
    {
        net->simulateTick();
    }

    #ifdef VERBOSE
        net->printHeaders();
        net->dumpMatrix();
    #endif

    delete net;

    return 0;
};