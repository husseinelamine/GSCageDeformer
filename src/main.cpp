#include "Tetrahedron.h"
#include "PointCloud.h"
#include "TopologyMapper.h"
#include "Renderer.h"
#include "MeshReader.h"
#include "TetrahedronMeshRenderer.h"

//#define HIDE_CONSOLE_WINDOW 1

// check if windows os
#ifdef _WIN32
#include <windows.h>
#endif

// with args
int main(int argc, char* argv[]) {

#ifdef _WIN32
#ifdef HIDE_CONSOLE_WINDOW
	// hide console window
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
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
	if (renderer.init() == -1)
	{
		return -1;
	}

	MeshReader meshReader;
	Mesh mesh;
	meshReader.readMsh("C:/Users/helamine/source/repos/ISProjectsV1/ISPhysics/assets/meshes/mug/surface_cage.obj_.msh", mesh);

	meshReader.reset();
	Mesh mesh2;
	meshReader.readMsh("C:/Users/helamine/source/repos/ISProjectsV1/ISPhysics/assets/meshes/mug/surface_cage.obj_.msh", mesh2);

	std::vector<Tetrahedron> tetrahedrons = meshReader.getTetrahedrons(mesh);
	TetCage cage{ tetrahedrons };
	cage.init();

	TetrahedronMeshRenderer tetrahedronMeshRenderer{ &cage };
	renderer.addMeshRenderer(&tetrahedronMeshRenderer);

	renderer.setupBuffers();

	while (!renderer.shouldClose())
	{
		renderer.render();
	}


 
	return 0;
}