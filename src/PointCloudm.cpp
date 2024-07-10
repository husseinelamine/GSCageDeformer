#include "PointCloudm.h"

namespace GSDeformer {

void PointCloud::addPoint(const Eigen::Vector3f& p) {
	points.push_back(p);
}

void PointCloud::resize(int size) {
	points.resize(size);
}

} // namespace GSDeformer