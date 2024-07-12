#include "TopologyMapper.h"
#include <iostream>

int TopologyMapper::findTetrahedron(GSDeformer::PointCloud& pc, TetCage& cage) {

	for (int i = 0; i < cage.tetrahedrons.size(); i++) {
		if (cage.tetrahedrons[i].contains(pc[0]))
			return i;
	}
	return -1;
}

bool TopologyMapper::barycentricPCtoTetCage(GSDeformer::PointCloud& pc, TetCage& cage, GSDeformer::PointCloud& barycentricPC) {
	bool isValid = true;
	barycentricPC.resize(pc.size());
	for (int i = 0; i < pc.size(); i++) {
		int tetIndex = findTetrahedron(pc, cage);
		if (tetIndex != -1) {
			barycentricPC.addPoint(cage.tetrahedrons[tetIndex].invTransposeMatrix * (pc[i] - cage.tetrahedrons[tetIndex].vertices[0]));
		}
		else {
			barycentricPC.addPoint(Eigen::Vector3f(-1, -1, -1));
			std::cout << "[WARNING]: Point " << i << " is not in any tetrahedron" << std::endl;
			isValid = false;
		}
	}
	return isValid;
}

Eigen::Vector4f TopologyMapper::findTetrahedron(Gaussian& gs, TetCage& cage) {
	for (int i = 0; i < cage.tetrahedrons.size(); i++) {
		Eigen::Vector3f bary = cage.tetrahedrons[i].contains(gs);
		if (bary[0] != -1)
			return Eigen::Vector4f(bary[0], bary[1], bary[2], i);
	}
	return Eigen::Vector4f(-1, -1, -1, -1);
}

bool TopologyMapper::barycentricGStoTetCage(GaussianCloud& gs, TetCage& cage, std::vector<Eigen::Vector4f>& barycentricGS) {
	if (cage.tetrahedrons.size() == 0) {
		return barycentricGStoTetCageFromEmptyCage(gs, cage, barycentricGS);
	}
	cage.init();
	bool isValid = true;
	int numInvalid = 0;
	barycentricGS.resize(gs.size());
	std::vector<Gaussian>& gsVec = gs.GetGaussianVec();
	for (int i = 0; i < gs.size(); i++) {
		Eigen::Vector4f bary = findTetrahedron(gsVec[i], cage);
		int tetIndex = bary[3];
		if (tetIndex != -1) {
			/*Eigen::Vector3f gsi = {gsVec[i].position[0], gsVec[i].position[1], gsVec[i].position[2]};
			auto tmp = cage.tetrahedrons[tetIndex].invTransposeMatrix * (-gsi + cage.tetrahedrons[tetIndex].vertices[0]);
			barycentricGS[i] = Eigen::Vector4f(tmp[0], tmp[1], tmp[2], tetIndex);*/
			barycentricGS[i] = bary;

			// check calculated barycentric is valid by comparing the point with the barycentric multiplied by the vertices
			Eigen::Vector3f p = cage.tetrahedrons[tetIndex].vertices[0] * bary[0] + cage.tetrahedrons[tetIndex].vertices[1] * bary[1] + cage.tetrahedrons[tetIndex].vertices[2] * bary[2] + cage.tetrahedrons[tetIndex].vertices[3] * bary[3];
			Eigen::Vector3f oldp = { gsVec[i].position[0], gsVec[i].position[1], gsVec[i].position[2] };

			float diff = (p - oldp).norm();
			diff = diff;
		}
		else {
			barycentricGS[i] = Eigen::Vector4f(-1, -1, -1, -1);
			//std::cout << "[WARNING]: Point " << i << " is not in any tetrahedron" << std::endl;
			isValid = false;
			numInvalid++;
		}
	}
	numInvalid;
	return isValid;
}

#define BB_DIST 20.0f

bool TopologyMapper::barycentricGStoTetCageFromEmptyCage(GaussianCloud& gs, TetCage& cage, std::vector<Eigen::Vector4f>& barycentricGS) {
	// construct cage by calculate a 8 points bbox then assign points in order to get tetrahderons.

	std::vector<Eigen::Vector3f> bbox;
	bbox.resize(8);

	float minX = gs.minX - BB_DIST; // -9.97
	float minY = gs.minY - BB_DIST; // 0.641
	float minZ = gs.minZ - BB_DIST; // -10.47
	float maxX = gs.maxX + BB_DIST; // 11.38
	float maxY = gs.maxY + BB_DIST; // 22.08
	float maxZ = gs.maxZ + BB_DIST; // 7.61

	// hardcoded bbox larger than this one 
	// to make sure all points are inside the bbox



	bbox[0] = Eigen::Vector3f(minX, minY, minZ);
	bbox[1] = Eigen::Vector3f(maxX, minY, minZ);
	bbox[2] = Eigen::Vector3f(maxX, maxY, minZ);
	bbox[3] = Eigen::Vector3f(minX, maxY, minZ);
	bbox[4] = Eigen::Vector3f(minX, minY, maxZ);
	bbox[5] = Eigen::Vector3f(maxX, minY, maxZ);
	bbox[6] = Eigen::Vector3f(maxX, maxY, maxZ);
	bbox[7] = Eigen::Vector3f(minX, maxY, maxZ);
	
	
	/*
	[0,1,2,6]
	[0,2,3,6]
	[0,3,7,6]
	[0,7,4,6]
	[0,4,5,6]
	[0,5,1,6]
	*/

	// construct 6 tetrahedrons
	std::vector<Tetrahedron> tetrahedrons{
		Tetrahedron(bbox[0], bbox[1], bbox[2], bbox[6]),
		Tetrahedron(bbox[0], bbox[2], bbox[3], bbox[6]),
		Tetrahedron(bbox[0], bbox[3], bbox[7], bbox[6]),
		Tetrahedron(bbox[0], bbox[7], bbox[4], bbox[6]),
		Tetrahedron(bbox[0], bbox[4], bbox[5], bbox[6]),
		Tetrahedron(bbox[0], bbox[5], bbox[1], bbox[6])
	};

	//float size = 65.0f;
	//// construct 1 super tetrahedron with large numbers insuring all points are inside
	//std::vector<Tetrahedron> tetrahedrons{
	//	Tetrahedron(-size* Eigen::Vector3f::Ones(), size * Eigen::Vector3f::UnitX(), size * Eigen::Vector3f::UnitY(), size * Eigen::Vector3f::UnitZ())
	//};

	TetCage cage_ { tetrahedrons };
	cage_.init();

	cage = cage_;

	return barycentricGStoTetCage(gs, cage_, barycentricGS);

}