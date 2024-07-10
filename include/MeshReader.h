#ifndef MESHREADER_H
#define MESHREADER_H

#include <string>
#include <vector>
#include <unordered_map>
#include "Tetrahedron.h"


struct Node {
	float x, y, z;
};

struct Element {
	int type;
	std::vector<int> tags;
	std::vector<int> nodes;
};

struct Mesh {
	std::vector<Node> nodes;
	std::vector<Element> elements;

};

class MeshReader
{

public:
	MeshReader(){}
	~MeshReader(){}

	void readMsh(const std::string& filename, Mesh& mesh);
	void readObj(const std::string& filename);

	void inline test() { /*readMsh("somepath.msh"); readMsh("anotherpath.obj"); */}

	std::vector<float> getVertices() const;
	std::vector<unsigned int> getIndices() const;
	std::vector<Tetrahedron> getTetrahedrons(const Mesh& mesh);

	void reset();

private:
	std::vector<float> m_vertices;
	std::vector<unsigned int> m_indices;

	std::vector<Node> nodes;
	std::unordered_map<int, Element> elements;



};

#endif // MESHREADER_H