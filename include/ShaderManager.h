#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include "BasicShaderProgram.h"
#include <unordered_map>
#include <string>
#include <functional>
#include <iostream>

class ShaderManager {
public:
    ShaderManager();
    ~ShaderManager();

    void loadShader(const std::string& name);
    ShaderProgram* getShader(const std::string& name) const;
    void useShader(const std::string& name) const;
    void setAllUniforms();

    inline static void init() {
        if (shaderMap.empty()) {
            shaderMap["basic"] = []() -> ShaderProgram* {
                BasicShaderProgram *program = new BasicShaderProgram();
                std::cout << program->getID() << std::endl;
                return program;
            };
        }
	}

private:
    std::unordered_map<std::string, ShaderProgram*> shaders;
    // explicit map of shader name and class to be instantiated
    static std::unordered_map<std::string, std::function<ShaderProgram* ()>> shaderMap;

};

#endif // SHADER_MANAGER_H
