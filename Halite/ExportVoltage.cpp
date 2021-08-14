#include "ExportVoltage.hpp"

ExportVoltage::ExportVoltage(size_t node) : node(node) { }

double ExportVoltage::value() { return voltage; }

void ExportVoltage::extract (MNASystem & m) {
    voltage = m.b[node].lu;
}
