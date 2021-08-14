#include <Halite/Halite.hpp>

#include <lv2plugin.hpp>
#include <Eigen/Dense>
#include <iostream>
#include <complex>
#include <random>

using namespace Eigen;
using namespace LV2;

class SpiceLV2 : public Plugin<SpiceLV2> {
private:
    NetList* net;
public:
    SpiceLV2(double rate) : Plugin<SpiceLV2>(1) {
        net = setupCircuit();
    }

    void run(uint32_t nframes) {
        for (uint32_t i = 0; i < nframes; ++i) {
            net->simulateTick();
            p(2)[i] = 0; p(3)[i] = 0;
        }
    }
};

static int _ = SpiceLV2::register_class("https://github.com/forked-from-1kasper/halite2");