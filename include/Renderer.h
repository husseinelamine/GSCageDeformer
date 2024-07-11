
#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "MeshRenderer.h"
#include "ShaderManager.h"
#include "TopologyMapper.h"
#include "PointCloud.h"
#include <Eigen/Dense>

class Renderer {
public:
    Renderer(int width = 720, int height = 480);
    ~Renderer();
    void render();
    int init();
    void update();
    void cleanup();
    bool shouldClose() { return glfwWindowShouldClose(window); }
    void setupShaders();
    void setupBuffers();
    GLFWwindow* getWindow() { return window; }

    void bindKeyCallback(GLFWkeyfun callback) {
    	glfwSetKeyCallback(window, callback);
    }

    void addMeshRenderer(MeshRenderer* meshRenderer);

private:
    int width, height;
    GLFWwindow* window;
    ShaderManager shaderManager;
    std::vector<MeshRenderer*> meshRenderers;

};

#endif // RENDERER_H
