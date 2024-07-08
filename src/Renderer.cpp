#include "Renderer.h"
#include <iostream>
#include "UtilMath.h"



const GLfloat colors[4][3] = {
	{ 1.0, 0.0, 0.0 }, // Red
	{ 0.0, 1.0, 0.0 }, // Blue
	{ 0.0, 0.0, 1.0 }, // Green
	{ 1.0, 1.0, 1.0 } // White
};

const GLuint indicesBase[] = {
	0, 1, 2,
	0, 2, 3,
	0, 3, 1,
	1, 3, 2

};

void Renderer::initCage() {
	std::vector<Tetrahedron> tetrahedrons{
		// Tetrahedron 1
		{
			{ 0.8, 0.8, 0.8},
			{ -0.8, -0.8, 0.8},
			{ -0.8, 0.8, -0.8},
			{ 0.8, -0.8, -0.8}
		}
	};

	cage = new TetCage{ tetrahedrons };
	cage->init();

	//const std::vector<Eigen::Vector3f> points{
	//    {0.5, 0.23, 0.78},
	//    {0.1, 0.2, 0.3},
	//    {0.7, 0.8, 0.9},
	//    {0.4, 0.5, 0.6}
	//};

	//PointCloud pc{ points };

	//PointCloud barycentricPC;
	//TopologyMapper::barycentricPCtoTetCage(pc, cage, barycentricPC);
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

	// Make the window's context current
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	// bind esc key to close the window
	bindKeyCallback([](GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	});

	glEnable(GL_DEPTH_TEST);


	// Initialize the cage
	initCage();

	// Setup OpenGL buffers and shaders
	setupBuffers();
	setupShaders();

	return 1;
}

void Renderer::setupBuffers() {
	// Create and bind VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Create and bind VBO and EBO
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &colorVBO);
	glGenBuffers(1, &EBO);

	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;
	//initCage();
	int tetIndex = 0;
	for (const auto& tetrahedron : cage->tetrahedrons) {
		for (const auto& vertex : tetrahedron.vertices) {
			vertices.push_back(vertex.x());
			vertices.push_back(vertex.y());
			vertices.push_back(vertex.z());
		}
		
		for(int i = 0; i < 12; i++) {
			indices.push_back(indicesBase[i] + tetIndex * 4);
		}

		tetIndex++;
	}

	// Bind and set VBO data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);


	// Bind and set EBO data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	// Set vertex attribute pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Set color attribute pointers
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);


	// Unbind VAO
	glBindVertexArray(0);
}

void Renderer::setupShaders() {
	// Compile and link shaders
	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
	shaderProgram = linkProgram(vertexShader, fragmentShader);
}

GLuint Renderer::compileShader(GLenum type, const char* source) {
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	return shader;
}

GLuint Renderer::linkProgram(GLuint vertexShader, GLuint fragmentShader) {
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	GLint success;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return shaderProgram;
}

void Renderer::rotateCage(float angle, const Eigen::Vector3f& axis) {
	Eigen::Matrix3f rotation = Eigen::AngleAxisf(angle, axis).toRotationMatrix();
	for (auto& tetrahedron : cage->tetrahedrons) {
		for (auto& vertex : tetrahedron.vertices) {
			vertex = rotation * vertex;
		}
	}
}

void Renderer::render() {

	// 
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Get Time
	float time = glfwGetTime();

	
	

	// Use the shader program
	glUseProgram(shaderProgram);


	// send transformation matrix to shader
	// Create transformations
	Eigen::Matrix4f model = Eigen::Matrix4f::Identity();
	Eigen::Matrix4f view = Eigen::Matrix4f::Identity();
	Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();

	// Set the model, view and projection matrices
	// rotate model over time
	model = Eigen::Affine3f(Eigen::AngleAxisf(time, Eigen::Vector3f(0.5f, 1.0f, 0.0f))).matrix();

	view = Eigen::Affine3f(Eigen::Translation3f(0.0f, 0.0f, -5.0f)).matrix();

	// function definition in UtilMath.h
	projection = Eigen::perspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);


	GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
	GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
	GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model.data());
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view.data());
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection.data());


	// Bind VAO and draw the elements
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, cage->tetrahedrons.size() * 12, GL_UNSIGNED_INT, 0);

	// Swap buffers and poll events
	glfwSwapBuffers(window);
	glfwPollEvents();
}