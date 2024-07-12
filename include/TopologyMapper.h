#ifndef TOPOLOGYMAPPER_H
#define TOPOLOGYMAPPER_H

#include "Tetrahedron.h"
#include "PointCloudm.h"
#include "gaussiancloud.h"


typedef GaussianCloud::Gaussian Gaussian;

class TopologyMapper
{
	public:

		static bool barycentricPCtoTetCage(GSDeformer::PointCloud &pc, TetCage &cage, GSDeformer::PointCloud &barycentricPC);
		static int findTetrahedron(GSDeformer::PointCloud &pc, TetCage &cage);

		static bool barycentricGStoTetCage(GaussianCloud &gs, TetCage &cage, std::vector<Eigen::Vector4f> &barycentricGS);
		static int findTetrahedron(Gaussian &gs, TetCage &cage);

		static bool barycentricGStoTetCageFromEmptyCage(GaussianCloud &gs, TetCage&cage, std::vector<Eigen::Vector4f> &barycentricGS);


};

#endif // TOPOLOGYMAPPER_H