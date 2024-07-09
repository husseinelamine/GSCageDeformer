#include "MeshReader.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <map>
#include <cstdint>




void MeshReader::readMsh(const std::string& filename, Mesh& mesh) {

    // check if filename is of extension .msh
    if (filename.substr(filename.find_last_of(".") + 1) != "msh") {
		throw std::runtime_error("Error: File is not of extension .msh");
	}

    std::ifstream fin(filename);
    if (!fin) {
        throw std::runtime_error("Error opening " + filename + " to read msh file.");
    }

    std::string line;
    while (std::getline(fin, line)) {
        if (line == "$MeshFormat") {
            if (!std::getline(fin, line)) {
                throw std::runtime_error("Error reading mesh format.");
            }
            std::istringstream iss(line);
            float version;
            int file_type, data_size;
            iss >> version >> file_type >> data_size;

            if (file_type != 0) {
                throw std::runtime_error("Only ASCII format is supported.");
            }

            if (!std::getline(fin, line) || line != "$EndMeshFormat") {
                throw std::runtime_error("Error reading end of mesh format.");
            }
        }
        else if (line == "$Nodes") {
            int num_nodes;
            if (!(fin >> num_nodes)) {
                throw std::runtime_error("Error reading number of nodes.");
            }

            mesh.nodes.resize(num_nodes);
            for (int i = 0; i < num_nodes; ++i) {
                int index;
                float x, y, z;
                if (!(fin >> index >> x >> y >> z)) {
                    throw std::runtime_error("Error reading node data.");
                }
                mesh.nodes[index-1] = { x, y, z };
            }
            if (!(std::getline(fin, line) && std::getline(fin, line)) || line != "$EndNodes") {
                throw std::runtime_error("Error reading end of nodes.");
            }
        }
        else if (line == "$Elements") {
            int num_elements;
            if (!(fin >> num_elements)) {
                throw std::runtime_error("Error reading number of elements.");
            }

            mesh.elements.resize(num_elements); // Assuming tetrahedral elements
            for (int i = 0; i < num_elements; ++i) {
                int index, elem_type, tags;
                if (!(fin >> index >> elem_type >> tags)) {
                    throw std::runtime_error("Error reading element header.");
                }
                if (elem_type != 4) {
                    throw std::runtime_error("Unsupported element type.");
                }
                int t0, t1, t2, t3;
                
                if (!(fin >> t0 >> t1 >> t2 >> t3)) {
					throw std::runtime_error("Error reading element data.");
				}
                mesh.elements[index-1] = { elem_type, {tags}, {t0-1, t1-1, t2-1, t3-1} };   
            }

            if (!(std::getline(fin, line) && std::getline(fin, line)) || line != "$EndElements") {
                throw std::runtime_error("Error reading end of elements.");
            }
        }
        else if (line == "$ElementData") {
            break; // Skipping the entire ElementData section
            // Skipping the entire ElementData section
            //while (std::getline(fin, line) && line != "$EndElementData");
        }
    }

    fin.close();
}

std::vector<Tetrahedron> MeshReader::getTetrahedrons(const Mesh& mesh) {
	std::vector<Tetrahedron> tetrahedrons;
    for (const auto& element : mesh.elements) {
        if (element.type == 4) {
            // cast points to Eigen::Vector3f
            Eigen::Vector3f p1(mesh.nodes[element.nodes[0]].x, mesh.nodes[element.nodes[0]].y, mesh.nodes[element.nodes[0]].z);
            Eigen::Vector3f p2(mesh.nodes[element.nodes[1]].x, mesh.nodes[element.nodes[1]].y, mesh.nodes[element.nodes[1]].z);
            Eigen::Vector3f p3(mesh.nodes[element.nodes[2]].x, mesh.nodes[element.nodes[2]].y, mesh.nodes[element.nodes[2]].z);
            Eigen::Vector3f p4(mesh.nodes[element.nodes[3]].x, mesh.nodes[element.nodes[3]].y, mesh.nodes[element.nodes[3]].z);
			
            tetrahedrons.push_back({ p1, p2, p3, p4 });

		}
	}
	return tetrahedrons;
}