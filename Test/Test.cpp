#include <Halite/Halite.hpp>
#include <Halite/ExportVoltage.hpp>

ExportVoltage voltage(5);

void onTick(MNASystem & m) {
    printf("TIME = %f, VOLTAGE = %f\n", m.time, voltage.value());
}

int main() {
    auto net = getTestCircuit(&onTick);
    net->addExport(&voltage);

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