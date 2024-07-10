#include "TopologyMapper.h"
#include <iostream>

int TopologyMapper::findTetrahedron(GSDeformer::PointCloud& pc, TetCage& cage) {

	for(int i = 0; i < cage.tetrahedrons.size(); i++){
		if (cage.tetrahedrons[i].contains(pc[0]))
			return i;
	}
	return -1;
}

bool TopologyMapper::barycentricPCtoTetCage(GSDeformer::PointCloud& pc, TetCage& cage, GSDeformer::PointCloud& barycentricPC) {
	bool isValid = true;
	barycentricPC.resize(pc.size());
	for(int i = 0; i < pc.size(); i++){
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