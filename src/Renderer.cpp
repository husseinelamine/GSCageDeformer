#include "Renderer.h"
#include <iostream>

void Renderer::initCage() {
	std::vector<Tetrahedron> tetrahedrons{
		// Tetrahedron 1
		{
			{ 0.0f, 0.0f, 0.0f },
			{ 1.0f, 0.0f, 0.0f },
			{ 0.0f, 1.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f }
		},
		// Tetrahedron 2
		{
			{ 1.0f, 1.0f, 1.0f },
			{ 2.0f, 1.0f, 1.0f },
			{ 1.0f, 2.0f, 1.0f },
			{ 1.0f, 1.0f, 2.0f }
		},
		// Tetrahedron 3
		{
			{ -1.0f, 0.0f, 0.0f },
			{ 0.0f, -1.0f, 0.0f },
			{ 0.0f, 0.0f, -1.0f },
			{ 1.0f, 1.0f, 1.0f }
		},
		// Tetrahedron 4
		{
			{ 0.5f, 0.5f, 0.5f },
			{ 1.5f, 0.5f, 0.5f },
			{ 0.5f, 1.5f, 0.5f },
			{ 0.5f, 0.5f, 1.5f }
		},
		// Tetrahedron 5
		{
			{ 0.0f, 0.0f, 0.0f },
			{ 1.0f, 0.0f, 0.0f },
			{ 0.0f, 1.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f }
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
}

void Renderer::setupBuffers() {
	// Create and bind VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Create and bind VBO and EBO
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	//initCage();
	for (const auto& tetrahedron : cage->tetrahedrons) {
		for (const auto& vertex : tetrahedron.vertices) {
			vertices.push_back(vertex.x());
			vertices.push_back(vertex.y());
			vertices.push_back(vertex.z());
		}
		unsigned int baseIndex = vertices.size() / 3 - 4;
		indices.push_back(baseIndex + 0);
		indices.push_back(baseIndex + 1);
		indices.push_back(baseIndex + 2);
		indices.push_back(baseIndex + 0);
		indices.push_back(baseIndex + 1);
		indices.push_back(baseIndex + 3);
		indices.push_back(baseIndex + 1);
		indices.push_back(baseIndex + 2);
		indices.push_back(baseIndex + 3);
		indices.push_back(baseIndex + 2);
		indices.push_back(baseIndex + 0);
		indices.push_back(baseIndex + 3);
	}

	// Bind and set VBO data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	// Bind and set EBO data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	// Set vertex attribute pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

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


void Renderer::render() {
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// Use the shader program
	glUseProgram(shaderProgram);

	// Bind VAO and draw the elements
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, cage->tetrahedrons.size() * 12, GL_UNSIGNED_INT, 0);

	// Swap buffers and poll events
	glfwSwapBuffers(window);
	glfwPollEvents();
}