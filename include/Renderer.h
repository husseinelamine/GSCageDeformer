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
	GLuint VAO, VBO, EBO, colorVBO;
	GLuint shaderProgram;
	TetCage* cage;

	GLuint compileShader(GLenum type, const char* source);
	GLuint linkProgram(GLuint vertexShader, GLuint fragmentShader);
	void setupBuffers();
	void setupShaders();
	void initCage();
	void rotateCage(float angle, const Eigen::Vector3f& axis);

	// Vertex shader source code
	const char* vertexShaderSource = R"(
		#version 330 core
		layout(location = 0) in vec3 aPos;
		layout(location = 1) in vec3 aColor;

		out vec3 ourColor;

		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;

		void main() {
			gl_Position = projection * view * model * vec4(aPos, 1.0);
			ourColor = aColor;
		}
		)";

	// Fragment shader source code
	const char* fragmentShaderSource = R"(
		#version 330 core
		in vec3 ourColor;
		out vec4 FragColor;
		void main() {
			FragColor = vec4(ourColor, 1.0);
		}
		)";


};


#endif // RENDERER_H