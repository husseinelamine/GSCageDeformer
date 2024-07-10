#ifndef POINTCLOUD_H
#define POINTCLOUD_H

#include <vector>
#include <Eigen/Dense>

namespace GSDeformer{

class PointCloud
{
	public:
		PointCloud(){}
		PointCloud(const std::vector<Eigen::Vector3f>& points): points(points) {}
		~PointCloud(){}

		void addPoint(const Eigen::Vector3f& point);
		void resize(int size);
		int size() const { return points.size(); }

		std::vector<Eigen::Vector3f> points;
	
		Eigen::Vector3f& operator[](int i) {
			return points[i];
		}

		const Eigen::Vector3f& operator[](int i) const {
			return points[i];
		}
};

}
#endif // POINTCLOUD_H