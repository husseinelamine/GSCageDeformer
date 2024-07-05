#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Tetrahedron.h"
#include "PointCloud.h"
#include "TopologyMapper.h"


class Renderer {
public:
	Renderer(int width = 720, int height = 480) : width(width), height(height) {}
	~Renderer() {}
	void render();
	int init();
	void update();
	void cleanup();
	bool shouldClose() { return glfwWindowShouldClose(window); }

	void bindKeyCallback(GLFWkeyfun callback) {
		glfwSetKeyCallback(window, callback);
	}

private:
	int width, height;
	GLFWwindow* window;
	GLuint VAO, VBO, EBO;
	GLuint shaderProgram;
	TetCage* cage;

	GLuint compileShader(GLenum type, const char* source);
	GLuint linkProgram(GLuint vertexShader, GLuint fragmentShader);
	void setupBuffers();
	void setupShaders();
	void initCage();

	// Vertex shader source code
	const char* vertexShaderSource = R"(
		#version 330 core
		layout(location = 0) in vec3 aPos;
		void main() {
			gl_Position = vec4(aPos, 1.0);
		}
		)";

	// Fragment shader source code
	const char* fragmentShaderSource = R"(
		#version 330 core
		out vec4 FragColor;
		void main() {
			FragColor = vec4(0.5, 0.5, 0.2, 1.0);
		}
		)";

};


#endif // RENDERER_H