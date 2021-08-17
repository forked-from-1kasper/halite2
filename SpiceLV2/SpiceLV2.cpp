#include <Halite/Halite.hpp>
#include <Halite/ExportVoltage.hpp>

#include <lv2plugin.hpp>

#include <iostream>
#include <atomic>
#include <memory>

#include <thread>
#include <mutex>
#include <queue>

using namespace LV2;

const size_t bufferMaxCapacity = 1024 * 64;
const unsigned spiceDelay = 1;

NetList* net;
ExportVoltage result(5);

std::mutex monitor;
std::queue<double>* buffer;
std::atomic<size_t> measured;

void onTick(MNASystem & m) {
    while (measured.load() >= bufferMaxCapacity) {};

    monitor.lock();
    buffer->push(result.value());
    measured++;
    monitor.unlock();
}

void solverThread() {
    for (;;) net->simulateTick();
}

class SpiceLV2 : public Plugin<SpiceLV2> {
private:
    std::thread* spiceThread;
public:
    SpiceLV2(double rate) : Plugin<SpiceLV2>(4) {
        buffer = new std::queue<double>();
        measured = 0;

        net = getTestCircuit(&onTick);
        net->addExport(&result);

        spiceThread = new std::thread(solverThread);
        std::this_thread::sleep_for (std::chrono::seconds(spiceDelay));
    }

    void run(uint32_t nframes) {
        while (measured.load() < nframes);

        monitor.lock();

        for (uint32_t i = 0; i < nframes; ++i) {
            p(2)[i] = buffer->front(); p(3)[i] = 0;
            buffer->pop(); measured--;
        }

        monitor.unlock();
    }

    ~SpiceLV2() {
        delete spiceThread;
        delete buffer;
        delete net;
    }
};

static int _ = SpiceLV2::register_class("https://github.com/forked-from-1kasper/halite2");