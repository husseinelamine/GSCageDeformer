#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include "Shader.h"

class ShaderProgram {
public:
    ShaderProgram();
    ShaderProgram(const std::string& vertexSource, const std::string& fragmentSource);
    ~ShaderProgram();

    void init(const std::string& vertexSource, const std::string& fragmentSource);

    void use() const;
    GLuint getID() const;

    virtual void setUniforms();

private:
    GLuint programID;
    Shader* vertexShader;
    Shader* fragmentShader;

    void linkProgram();
    void checkLinkErrors() const;
};

#endif // SHADER_PROGRAM_H
