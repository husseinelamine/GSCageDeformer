#include "Tetrahedron.h"

// with args
int main(int argc, char* argv[]) {
	std::vector<Tetrahedron> tetrahedrons{
		{
			{ 0, 0, 0 },
			{ 1, 0, 0 },
			{ 0, 1, 0 },
			{ 0, 0, 1 }
		},
		{
			{ 0, 0, 0 },
			{ 1, 0, 0 },
			{ 0, 1, 0 },
			{ 0, 0, 1 }
		},
		{
			{ 0, 0, 0 },
			{ 1, 0, 0 },
			{ 0, 1, 0 },
			{ 0, 0, 1 }
		}
	};

	TetCage cage{ tetrahedrons };
	cage.init();

	return 0;
}