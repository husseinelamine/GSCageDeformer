#ifndef TETRAHEDRONMESHRENDERER_H
#define TETRAHEDRONMESHRENDERER_H

#include "MeshRenderer.h"
#include "Tetrahedron.h"
#include <vector>
#include <Eigen/Core>

class TetrahedronMeshRenderer : public MeshRenderer {
public:
    TetrahedronMeshRenderer(TetCage* cage);
    ~TetrahedronMeshRenderer();

    void setupBuffers() override;
    void render() override;

private:
    TetCage* cage;
    GLuint VAO, VBO, EBO;
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
};

#endif // TETRAHEDRONMESHRENDERER_H
