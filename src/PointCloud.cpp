#include "PointCloud.h"


PointCloud::addPoint(Point p) {
	points.push_back(p);
}

PointCloud::resize(int size) {
	points.resize(size);
}