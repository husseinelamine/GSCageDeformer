#include "Tetrahedron.h"
#include "UtilMath.h"

using namespace UtilMath;

void Tetrahedron::init() {
    // Calculate the center of the tetrahedron
    center = Eigen::Vector3f::Zero();
    for (int i = 0; i < 4; i++) {
        center += vertices[i];
    }
    center *= 0.25;

    // Calculate edge matrix
    for (int i = 0; i < 3; i++) {
        edgeMatrix.col(i) = vertices[(i + 1) % 4] - vertices[i];
    }

    // Calculate the inverse transpose of the edge matrix
    invTransposeMatrix = edgeMatrix.inverse().transpose();
}

bool Tetrahedron::isValidBarycentric(const Eigen::Vector3f& bary) const {
    
    return bary[0] >= 0 && bary[1] >= 0 && bary[2] >= 0 && isSumNearlyGE(bary[0], bary[1], bary[2], 1.0f);

}

bool Tetrahedron::contains(const Eigen::Vector3f& p) const {
    // Returns true when the first point is found inside the tetrahedron

    Eigen::Vector3f AP = p - vertices[0];
    Eigen::Vector3f bary = invTransposeMatrix * AP;

    if (isValidBarycentric(bary)) {
        return true;
    }

    return false;
	
    
}

void TetCage::init() {
    for (Tetrahedron& t : tetrahedrons) {
        t.init();
    }
}

//int Tetrahedron::contains(const Eigen::Vector3f& p) const {
//    // having base point A, inverseTrasnpose * AP 
//    Eigen::Vector3f AP = p - vertices[0];
//    Eigen::Vector3f bary = invTransposeMatrix * AP;
//    float d = std::min(std::min(bary[0], bary[1]), std::min(bary[2], 1 - (bary[0] + bary[1] + bary[2])));
//    if (d > 0) {
//        // calculate norm2 of center to p
//        d = (center - p).norm();
//    }
//    return d;
//}