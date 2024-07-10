#include "ShaderProgram.h"
#include <iostream>

ShaderProgram::ShaderProgram(const std::string& vertexSource, const std::string& fragmentSource) {
    vertexShader = new Shader(GL_VERTEX_SHADER, vertexSource);
    fragmentShader = new Shader(GL_FRAGMENT_SHADER, fragmentSource);
    programID = glCreateProgram();
    glAttachShader(programID, vertexShader->getID());
    glAttachShader(programID, fragmentShader->getID());
    linkProgram();
}

ShaderProgram::ShaderProgram() {
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(programID);
    delete vertexShader;
    delete fragmentShader;
}

void ShaderProgram::init(const std::string& vs, const std::string& fs) {
    vertexShader = new Shader(GL_VERTEX_SHADER, vs);
	fragmentShader = new Shader(GL_FRAGMENT_SHADER, fs);
	programID = glCreateProgram();
	glAttachShader(programID, vertexShader->getID());
	glAttachShader(programID, fragmentShader->getID());
	linkProgram();

}

void ShaderProgram::use() const {
    glUseProgram(programID);
}

GLuint ShaderProgram::getID() const {
    return programID;
}

void ShaderProgram::linkProgram() {
    glLinkProgram(programID);
    checkLinkErrors();
}

void ShaderProgram::checkLinkErrors() const {
    GLint success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(programID, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
}

void ShaderProgram::setUniforms() {
	// Set uniforms here
}