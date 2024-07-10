#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <string>

class Shader {
public:
    Shader(GLenum type, const std::string& source);
    ~Shader();

    GLuint getID() const;

private:
    GLuint shaderID;

    void compile(const std::string& source);
    void checkCompileErrors() const;
};

#endif // SHADER_H
