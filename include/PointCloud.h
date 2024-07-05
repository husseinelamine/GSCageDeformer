#ifndef POINTCLOUD_H
#define POINTCLOUD_H

#include <vector>
#include <Eigen/Dense>

class PointCloud
{
	public:
		PointCloud();
		PointCloud(const std::vector<Eigen::Vector3f>& points): points(points) {}
		~PointCloud();

		void addPoint(const Eigen::Vector3f& point);
		void resize(int size);

	std::vector<Eigen::Vector3f> points;
};


#endif // POINTCLOUD_H