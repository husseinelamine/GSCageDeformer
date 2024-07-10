#include "Renderer.h"
#include "ShaderManager.h"
#include <iostream>
#include "UtilMath.h"
#include "MeshReader.h"


Renderer::Renderer(int width, int height) : width(width), height(height) {}

Renderer::~Renderer() {
}

int Renderer::init() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(EXIT_FAILURE);
    }

    window = glfwCreateWindow(width, height, "GSCageDeformer", NULL, NULL);

    if (!window) {
        glfwTerminate();
        std::cerr << "Failed to create GLFW window" << std::endl;
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    bindKeyCallback([](GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
    });

    glEnable(GL_DEPTH_TEST);


    setupBuffers();
    setupShaders();

    return 1;
}

void Renderer::setupShaders() {
    ShaderManager::init();
    shaderManager.loadShader("basic");
}

void Renderer::setupBuffers() {
    for (auto meshRenderer : meshRenderers) {
        meshRenderer->setupBuffers();
    }
}

void Renderer::addMeshRenderer(MeshRenderer* meshRenderer) {
    meshRenderers.push_back(meshRenderer);
}

void Renderer::render() {
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float time = glfwGetTime();

    shaderManager.useShader("basic");

    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();

    model = Eigen::Affine3f(Eigen::AngleAxisf(time, Eigen::Vector3f(0.f, 1.0f, 0.0f))).matrix();
    view = Eigen::Affine3f(Eigen::Translation3f(0.0f, 0.0f, -100.0f)).matrix();
    projection = Eigen::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);

    GLint modelLoc = glGetUniformLocation(shaderManager.getShader("basic")->getID(), "model");
    GLint viewLoc = glGetUniformLocation(shaderManager.getShader("basic")->getID(), "view");
    GLint projLoc = glGetUniformLocation(shaderManager.getShader("basic")->getID(), "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model.data());
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view.data());
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection.data());

    for (auto meshRenderer : meshRenderers) {
        meshRenderer->render();
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
}
