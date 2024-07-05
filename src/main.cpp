#include "Tetrahedron.h"
#include "PointCloud.h"
#include "TopologyMapper.h"
#include "Renderer.h"

// check if windows os
#ifdef _WIN32
#include <windows.h>
#endif

// with args
int main(int argc, char* argv[]) {

#ifdef _WIN32
	// hide console window
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif

	/*//std::vector<Tetrahedron> tetrahedrons{
	//	{
	//		{ 0, 0, 0 },
	//		{ 1, 0, 0 },
	//		{ 0, 1, 0 },
	//		{ 0, 0, 1 }
	//	},
	//	{
	//		{ 0, 0, 0 },
	//		{ 1, 0, 0 },
	//		{ 0, 1, 0 },
	//		{ 0, 0, 1 }
	//	},
	//	{
	//		{ 0, 0, 0 },
	//		{ 1, 0, 0 },
	//		{ 0, 1, 0 },
	//		{ 0, 0, 1 }
	//	}
	//};

	//TetCage cage{ tetrahedrons };
	//cage.init();

	//const std::vector<Eigen::Vector3f> points{
	//	{0.5, 0.23, 0.78},
	//	{0.1, 0.2, 0.3},
	//	{0.7, 0.8, 0.9},
	//	{0.4, 0.5, 0.6}
	//};

	//PointCloud pc{points};

	//PointCloud barycentricPC;
	//TopologyMapper::barycentricPCtoTetCage(pc, cage, barycentricPC);*/

	Renderer renderer{ 720, 480 }; 
	renderer.init();

	while (!renderer.shouldClose())
	{
		renderer.render();
	}


 
	return 0;
}