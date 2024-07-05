#include "TopologyMapper.h"

int TopologyMapper::findTetrahedron(PointCloud& pc, TetCage& cage) {

	int tetIndex = -1;
	for(int i = 0; i < cage.tetrahedra.size(); i++){
		if(cage.tetrahedra[i].contains(pc[0])){
			tetIndex = i;
			break;
		}
	}
	return tetIndex;


}

TopologyMapper::barycentricPCtoTetCage(PointCloud& pc, TetCage& cage, PointCloud& barycentricPC) {
	barycentricPC.resize(pc.size());
	for(int i = 0; i < pc.size(); i++){
		barycentricPC[i] = cage.barycentricCoordinates(pc[i]);
	}
}