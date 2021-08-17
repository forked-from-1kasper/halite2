#include <Halite/Halite.hpp>
#include <Halite/ExportVoltage.hpp>

ExportVoltage voltage(5);

void onTick(IMNASystem & m) {
}

int main() {
    auto net = getTestCircuit(&onTick);
    net->addExport(&voltage);

    #ifdef VERBOSE
        net->printHeaders();
    #endif

    for (int i = 0; i < 16; ++i)
        net->tick();

    #ifdef VERBOSE
        net->printHeaders();
    #endif

    delete net;

    return 0;
};