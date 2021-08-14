#include <Halite/Halite.hpp>

int main()
{
    auto net = setupCircuit();

    net->printHeaders();

    for(int i = 0; i < 16; ++i)
    {
        net->simulateTick();
    }
    net->printHeaders();

    net->dumpMatrix();

    delete net;

    return 0;
};