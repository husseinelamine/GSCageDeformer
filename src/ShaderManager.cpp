#include "ShaderManager.h"






std::unordered_map<std::string, std::function<ShaderProgram* ()>> ShaderManager::shaderMap;

ShaderManager::ShaderManager() {}

ShaderManager::~ShaderManager() {
	for (auto& pair : shaders) {
		delete pair.second;
	}
}

void ShaderManager::loadShader(const std::string& name) {
	auto it = shaderMap.find(name);
	if (it != shaderMap.end()) {
		// get function variable from map
		std::function<ShaderProgram* ()> func = it->second;
		// call function to instantiate class
		shaders[name] = func();

	}

}

ShaderProgram* ShaderManager::getShader(const std::string& name) const {
	auto it = shaders.find(name);
	if (it != shaders.end()) {
		return it->second;
	}
	return nullptr;
}

void ShaderManager::useShader(const std::string& name) const {
	ShaderProgram* shader = getShader(name);
	if (shader) {
		shader->use();
	}
}


void ShaderManager::setAllUniforms() {
	for (auto& pair : shaders) {
		pair.second->setUniforms();
	}
}