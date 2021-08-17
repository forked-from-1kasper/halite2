#include "ExportVoltage.hpp"

ExportVoltage::ExportVoltage(size_t node) : node(node) { }

double ExportVoltage::value() { return voltage; }

void ExportVoltage::extract (IMNASystem & m) {
    voltage = m.getValue(node);
}
