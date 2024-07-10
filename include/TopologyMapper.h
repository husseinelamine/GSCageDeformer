#ifndef TOPOLOGYMAPPER_H
#define TOPOLOGYMAPPER_H

#include "Tetrahedron.h"
#include "PointCloudm.h"


class TopologyMapper
{
	public:

		static bool barycentricPCtoTetCage(GSDeformer::PointCloud &pc, TetCage &cage, GSDeformer::PointCloud &barycentricPC);
		static int findTetrahedron(GSDeformer::PointCloud &pc, TetCage &cage);
};

#endif // TOPOLOGYMAPPER_H