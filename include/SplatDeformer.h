//SplatDeformer.h
#ifndef SPLATDEFORMER_H
#define SPLATDEFORMER_H

#include "gaussiancloud.h"
#include "Tetrahedron.h"


#include <vector>
#include <Eigen/Dense>

class SplatDeformer {

public:
	inline static void deformGaussianCloud(GaussianCloud& gaussianCloud, std::vector<Eigen::Vector4f>& barycentricGS, TetCage &cage) {
		for (int i = 0; i < gaussianCloud.GetGaussianVec().size(); i++) {
			int tetIndex = barycentricGS[i][3];
			if (barycentricGS[i][0] != -1) {
				float pos[3] = { gaussianCloud.GetGaussianVec()[i].position[0], gaussianCloud.GetGaussianVec()[i].position[1], gaussianCloud.GetGaussianVec()[i].position[2] };
				Eigen::Vector4f bary = {barycentricGS[i][0], barycentricGS[i][1], barycentricGS[i][2], 1 - barycentricGS[i][0] - barycentricGS[i][1] - barycentricGS[i][2]};
				for (int j = 0; j < 3; j++) {
					gaussianCloud.GetGaussianVec()[i].position[j] = cage.tetrahedrons[tetIndex].vertices[0][j] * bary[0] + cage.tetrahedrons[tetIndex].vertices[1][j] * bary[1] + cage.tetrahedrons[tetIndex].vertices[2][j] * bary[2] + cage.tetrahedrons[tetIndex].vertices[3][j] * bary[3];
				}
				float afterPos[3]= { gaussianCloud.GetGaussianVec()[i].position[0], gaussianCloud.GetGaussianVec()[i].position[1], gaussianCloud.GetGaussianVec()[i].position[2] };
				int a = 0;
			}
			else {
				float time = glfwGetTime() * 0.01f;
				// check odd or even
				if ((int)(time*100.0) % 2 == 0) {
					gaussianCloud.GetGaussianVec()[i].position[0] += time;
					gaussianCloud.GetGaussianVec()[i].position[1] += time;
					gaussianCloud.GetGaussianVec()[i].position[2] += time;
				}
				else {
					gaussianCloud.GetGaussianVec()[i].position[0] -= time;
					gaussianCloud.GetGaussianVec()[i].position[1] -= time;
					gaussianCloud.GetGaussianVec()[i].position[2] -= time;
				}
			}
		}
	}
};


#endif // SPLATDEFORMER_H