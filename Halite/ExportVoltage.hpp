#pragma once

#include "Halite.hpp"

class ExportVoltage : public Export<double> {
public:
    ExportVoltage(size_t node);
    double value() final;
    void extract (IMNASystem & m) final;
private:
    double voltage; size_t node;
};
