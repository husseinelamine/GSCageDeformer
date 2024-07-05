import gmsh
import argparse
import numpy as np

def init():
    # Initialiser gmsh
    gmsh.initialize()

def read(tetra_mesh_path):
    gmsh.open(tetra_mesh_path)

def map_to_GSsplats():
    # Récupérer les coordonnées des nœuds
    node_tags, node_coords, _ = gmsh.model.mesh.getNodes()
    node_coords = node_coords.reshape((-1, 3))  # Transformer en un tableau de Nx3

    # Dictionnaire pour accéder rapidement aux coordonnées des nœuds
    node_dict = {tag: coord for tag, coord in zip(node_tags, node_coords)}

    # Listes pour stocker les bases et centres des tétraèdres
    bases = []
    centers = []

    # Récupérer tous les éléments du maillage
    element_types, element_tags, element_node_tags = gmsh.model.mesh.getElements()

    for element_type, tags, nodes in zip(element_types, element_tags, element_node_tags):
        # Type d'élément 4 correspond aux tétraèdres dans Gmsh
        if element_type == 4:
            nodes = nodes.reshape((-1, 4))  # Transformer en un tableau de Mx4
            for tet_nodes in nodes:
                # Récupérer les coordonnées des nœuds du tétraèdre
                tet_coords = np.array([node_dict[node] for node in tet_nodes])
                # Calculer la matrice de base
                m = np.zeros((3, 3))
                m[:, 0] = tet_coords[1] - tet_coords[0]
                m[:, 1] = tet_coords[2] - tet_coords[0]
                m[:, 2] = tet_coords[3] - tet_coords[0]
                mt = np.transpose(m)
                basis = np.linalg.inv(mt)
                bases.append(basis)
                # Calculer le centre du tétraèdre
                center = np.mean(tet_coords, axis=0)
                centers.append(center)

    # Afficher les bases et centres des tétraèdres
    for i, (basis, center) in enumerate(zip(bases, centers)):
        print(f"Tetrahedron {i+1}: Basis =\n{basis}\nCenter = {center}")

    return bases, centers

def finalize():
    # Finaliser gmsh
    gmsh.finalize()

if __name__ == "__main__":
    args = argparse.ArgumentParser(description="Map a tetrahedral mesh to GSplats")
    args.add_argument("--tetra_mesh", help="Path to the tetrahedral mesh file", default="C:/Users/helamine/source/repos/ISProjectsV1/ISPhysics/assets/meshes/mug/surface_cage.obj_.msh")
    args = args.parse_args()
    
    init()
    read(args.tetra_mesh)
    map_to_GSsplats()
    finalize()
