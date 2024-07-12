#include "Tetrahedron.h"
#include "UtilMath.h"

using namespace UtilMath;


TetCage::TetCage(std::shared_ptr<GaussianCloud> cloud) {

}

void Tetrahedron::init() {
    // Calculate the center of the tetrahedron
    center = Eigen::Vector3f::Zero();
    for (int i = 0; i < 4; i++) {
        center += vertices[i];
    }
    center *= 0.25;

    // Calculate edge matrix
    edgeMatrix.col(0) = vertices[1] - vertices[0];
    edgeMatrix.col(1) = vertices[2] - vertices[0];
    edgeMatrix.col(2) = vertices[3] - vertices[0];


    // Calculate the inverse transpose of the edge matrix
    invTransposeMatrix = edgeMatrix.inverse().transpose();
    //invTransposeMatrix = edgeMatrix.inverse().transpose();
}

bool Tetrahedron::isValidBarycentric(const Eigen::Vector3f& bary) const {
    
    float b3 = 1.0f - bary[0] - bary[1] - bary[2];
    
    return bary[0] >= 0.0f && bary[1] >= 0.0f && bary[2] >= 0.0f && b3 >= 0.0f;

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

float scalarTripleProduct(const Eigen::Vector3f& a, const Eigen::Vector3f& b, const Eigen::Vector3f& c) {
	return a.dot(b.cross(c));
}

Eigen::Vector3f Tetrahedron::contains(const Gaussian& g) const {
    // Returns true when the first point is found inside the tetrahedron
    Eigen::Vector3f gpos{g.position[0], g.position[1], g.position[2] };
	Eigen::Vector3f AP = gpos - vertices[0];
	Eigen::Vector3f bary = invTransposeMatrix * AP;

    Eigen::Vector3f v0 = vertices[0];
    Eigen::Vector3f v1 = vertices[1];
    Eigen::Vector3f v2 = vertices[2];
    Eigen::Vector3f v3 = vertices[3];

    Eigen::Vector3f vap = gpos - v0;
    Eigen::Vector3f vbp = gpos - v1;

    Eigen::Vector3f vab = v1 - v0;
    Eigen::Vector3f vac = v2 - v0;
    Eigen::Vector3f vad = v3 - v0;

    Eigen::Vector3f vbc = v2 - v1;
    Eigen::Vector3f vbd = v3 - v1;

    float va6 = scalarTripleProduct(vbp, vbd, vbc);
    float vb6 = scalarTripleProduct(vap, vac, vad);
    float vc6 = scalarTripleProduct(vap, vad, vab);
    float vd6 = scalarTripleProduct(vap, vab, vac);
    float v6 = 1 / scalarTripleProduct(vab, vac, vad);


    // check if this calculation is correct
    Eigen::Vector3f bary2 = Eigen::Vector3f(va6 * v6, vb6 * v6, vc6 * v6);
    if (isValidBarycentric(bary2))
		return bary2;

    return Eigen::Vector3f(-1, -1, -1);
}

void TetCage::init() {
    for (Tetrahedron& t : tetrahedrons) {
        t.init();
    }
}

void TetCage::simplify() {
    // simplify preserving the outer tetrahedrons, limiting the number of tetrahedrons to 12
    for (int i = 0; i < tetrahedrons.size(); i++) {
        if (tetrahedrons[i].center.norm() > 20) {
			tetrahedrons.erase(tetrahedrons.begin() + i);
		}
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