#include "TransientSimulation.hpp"

void MNASystemEigen::setSize(int n) {
    A = new SparseMatrixXd(n - 1, n - 1);
    b = new Eigen::VectorXd(n - 1);
    x = new Eigen::VectorXd(n - 1);

    b->setZero(); x->setZero();

    vals.resize(n);
    nodes.resize(n);

    for (unsigned i = 0; i < n; ++i) {
        vals[i].clear();

        char buf[16];
        sprintf(buf, "v%d", i);
        nodes[i].name = buf;
        nodes[i].type = MNANodeInfo::tVoltage;
        nodes[i].scale = 1;
    }

    time = 0;
}

void pushBack(MNACells & vect, int i, int j, double g, double gtimed, double* gdyn) {
    vect.push_back(MNACell());
    vect.back().initPos(i, j);
    vect.back().g = g;
    vect.back().gtimed = gtimed;
    vect.back().gdyn = gdyn;
}

void MNASystemEigen::stamp(int i, int j, double g, double gtimed, double* gdyn) {
    if (i > 0 && j > 0)
        pushBack(conn, i - 1, j - 1, g, gtimed, gdyn);
}

void MNASystemEigen::stampRhs(int i, double g, double gtimed, double* gdyn) {
    if (i > 0) pushBack(vals, i - 1, 0, g, gtimed, gdyn);
}

double MNASystemEigen::getValue(int idx) {
    return (idx > 0) ? (*x)[idx - 1] : 0;
}

MNASystemEigen::~MNASystemEigen() {
    delete A; delete b; delete x;
}
