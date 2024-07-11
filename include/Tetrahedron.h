#ifndef TETRAHEDRON_H
#define TETRAHEDRON_H

#include <Eigen/Dense>
#include <vector>
#include "gaussiancloud.h"

typedef GaussianCloud::Gaussian Gaussian;

class Tetrahedron {
public:
    std::vector<Eigen::Vector3f> vertices;
    Eigen::Matrix3f edgeMatrix;
    Eigen::Matrix3f invTransposeMatrix;
    Eigen::Vector3f center;

    Tetrahedron(Eigen::Vector3f v0, Eigen::Vector3f v1, Eigen::Vector3f v2, Eigen::Vector3f v3)
        : vertices({ v0, v1, v2, v3 }) {}


    void init();
    bool contains(const Eigen::Vector3f& p) const; // returns p is inside or outside the tetrahedron
    bool contains(const Gaussian& g) const;
    bool isValidBarycentric(const Eigen::Vector3f& bary) const;
};

class TetCage {
public:

    TetCage() {}

    std::vector<Tetrahedron> tetrahedrons;

    TetCage(std::vector<Tetrahedron> tets) : tetrahedrons(tets) {}
    TetCage(std::shared_ptr<GaussianCloud> cloud);

    void simplify();


    void init();
};

#endif // TETRAHEDRON_H
