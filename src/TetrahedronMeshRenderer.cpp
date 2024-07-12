#include "TetrahedronMeshRenderer.h"
#include <iostream>

TetrahedronMeshRenderer::TetrahedronMeshRenderer(TetCage* cage) : cage(cage) {}

TetrahedronMeshRenderer::~TetrahedronMeshRenderer() {
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

void TetrahedronMeshRenderer::setupBuffers() {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    int tetIndex = 0;
    for (const auto& tetrahedron : cage->tetrahedrons) {
        for (const auto& vertex : tetrahedron.vertices) {
            vertices.push_back(vertex.x());
            vertices.push_back(vertex.y());
            vertices.push_back(vertex.z());
        }

        indices.push_back(tetIndex * 4);
        indices.push_back(tetIndex * 4 + 1);
        indices.push_back(tetIndex * 4 + 2);

        indices.push_back(tetIndex * 4);
        indices.push_back(tetIndex * 4 + 2);
        indices.push_back(tetIndex * 4 + 3);

        indices.push_back(tetIndex * 4);
        indices.push_back(tetIndex * 4 + 3);
        indices.push_back(tetIndex * 4 + 1);

        indices.push_back(tetIndex * 4 + 1);
        indices.push_back(tetIndex * 4 + 3);
        indices.push_back(tetIndex * 4 + 2);

        tetIndex++;
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void TetrahedronMeshRenderer::render() {
    glBindVertexArray(VAO);
    //glDrawElements(GL_TRIANGLES, cage->tetrahedrons.size() * 12, GL_UNSIGNED_INT, 0);
    glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
}
