#include "Eigen.hpp"

void MNACell::clear() {
    g = 0;
    gtimed = 0;
}

void MNACell::initPos(size_t row, size_t col) {
    m_row = row; m_col = col;
}

void MNACell::initLU(double stepScale)
{
    prelu = g + gtimed * stepScale;
}

void MNACell::updatePre()
{
    m_value = prelu + (gdyn != nullptr ? *gdyn : 0);
}
