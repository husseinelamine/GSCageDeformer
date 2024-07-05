#ifndef TOPOLOGYMAPPER_H
#define TOPOLOGYMAPPER_H

#include "Tetrahedron.h"
#include "PointCloud.h"


class TopologyMapper
{
	public:

		static void barycentricPCtoTetCage(PointCloud &pc, TetCage &cage, PointCloud &barycentricPC);
		static int findTetrahedron(PointCloud &pc, TetCage &cage);
};

#endif // TOPOLOGYMAPPER_H