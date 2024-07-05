#include "TopologyMapper.h"

int TopologyMapper::findTetrahedron(PointCloud& pc, TetCage& cage) {

	int tetIndex = -1;
	float d = -1;
	float distance = 1e10;
	for(int i = 0; i < cage.tetrahedrons.size(); i++){
		d = cage.tetrahedrons[i].contains(pc[0]);
		if(d != -1){
			if (d < distance) {
				distance = d;
				tetIndex = i;
			}	
		}
		
	}
	return tetIndex;


}

void TopologyMapper::barycentricPCtoTetCage(PointCloud& pc, TetCage& cage, PointCloud& barycentricPC) {
	barycentricPC.resize(pc.size());
	for(int i = 0; i < pc.size(); i++){
		int tetIndex = findTetrahedron(pc, cage);
		if (tetIndex != -1) {
			barycentricPC.addPoint(cage.tetrahedrons[tetIndex].invTransposeMatrix * (pc[i] - cage.tetrahedrons[tetIndex].vertices[0]));
		}
		else {
			barycentricPC.addPoint(Eigen::Vector3f(-1, -1, -1));
		}
	}
}