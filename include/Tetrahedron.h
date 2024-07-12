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
    Eigen::Vector3f contains(const Gaussian& g) const;
    bool isValidBarycentric(const Eigen::Vector3f& bary) const;
};

class TetCage {
public:

    TetCage() {}

    std::vector<Tetrahedron> tetrahedrons;

    TetCage(std::vector<Tetrahedron> tets) : tetrahedrons(tets) {}
    TetCage(std::shared_ptr<GaussianCloud> cloud);

    void simplify();

    static void deformCage(TetCage& cage) {
        float factor = 0.01;
        // bend some of the tetrahedrons
        for (int i = 0; i < cage.tetrahedrons.size(); i++) {
            if (i % 2 == 0) {
                cage.tetrahedrons[i].vertices[0][0] += factor;
                cage.tetrahedrons[i].vertices[1][1] += factor;
                cage.tetrahedrons[i].vertices[2][2] += factor;
                cage.tetrahedrons[i].vertices[3][0] += factor;
			}
            else {
                cage.tetrahedrons[i].vertices[0][0] -= factor;
				cage.tetrahedrons[i].vertices[1][1] -= factor;
				cage.tetrahedrons[i].vertices[2][2] -= factor;
				cage.tetrahedrons[i].vertices[3][0] -= factor;
            }
		}
        cage.init();
    }

    void init();
};

#endif // TETRAHEDRON_H
