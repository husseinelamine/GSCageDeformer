#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

Shader::Shader(GLenum type, const std::string& source) {
    shaderID = glCreateShader(type);
    compile(source);
}

Shader::~Shader() {
    glDeleteShader(shaderID);
}

GLuint Shader::getID() const {
    return shaderID;
}

void Shader::compile(const std::string& source) {
    const char* shaderCode = source.c_str();
    glShaderSource(shaderID, 1, &shaderCode, NULL);
    glCompileShader(shaderID);
    checkCompileErrors();
}

void Shader::checkCompileErrors() const {
    GLint success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
}
