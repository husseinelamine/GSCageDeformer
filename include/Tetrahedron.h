#ifndef TETRAHEDRON_H
#define TETRAHEDRON_H

#include <Eigen/Dense>
#include <vector>

class Tetrahedron {
public:
    std::vector<Eigen::Vector3d> vertices;
    Eigen::Matrix3d edgeMatrix;
    Eigen::Matrix3d invTransposeMatrix;
    Eigen::Vector3d center;

    Tetrahedron(Eigen::Vector3d v0, Eigen::Vector3d v1, Eigen::Vector3d v2, Eigen::Vector3d v3)
        : vertices({ v0, v1, v2, v3 }) {}


    void init();
    int contains(const Eigen::Vector3d& p) const; // returns -1 if p is outside, distance to the surface otherwise
};

class TetCage {
public:
    std::vector<Tetrahedron>& tetrahedrons;

    TetCage(std::vector<Tetrahedron>& tets) : tetrahedrons(tets) {}


    void init();
};

#endif // TETRAHEDRON_H
