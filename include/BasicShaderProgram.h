#ifndef BASIC_SHADER_PROGRAM_H
#define BASIC_SHADER_PROGRAM_H

#include "ShaderProgram.h"
#include <iostream>

class BasicShaderProgram: public ShaderProgram {

private:
	char* vertexShaderSource = R"(
		#version 330 core
		layout(location = 0) in vec3 aPos;

		out vec3 position;
		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;

		void main() {
			gl_Position = projection * view * model * vec4(aPos, 1.0);
			position = aPos;
		}
		)";

	// Fragment shader source code
	const char* fragmentShaderSource = R"(
		#version 330 core
		in vec3 position;
		out vec4 FragColor;
		void main() {
			vec4 ambientColor = vec4(0.8, 0.8, 0.8, 1.0);
			vec4 frontColor = vec4(0.0, 1.0, 0.0, 1.0);
			vec4 backColor = vec4(1.0, 0.0, 0.0, 1.0);
			vec3 lightPos = vec3(0.0, 0.0, 0.0);
			vec3 N = normalize(cross(dFdx(position), dFdy(position)));
			vec3 L = normalize(lightPos - position);
			float lambert = dot(N, L);
			if(gl_FrontFacing){
				FragColor = vec4(ambientColor.rgb + lambert * frontColor.rgb, frontColor.a);
			}
			else{
				FragColor = vec4(ambientColor.rgb + lambert * backColor.rgb, backColor.a);
			}
		}
		)";

	public:
	BasicShaderProgram(): ShaderProgram() {
		init(vertexShaderSource, fragmentShaderSource);
	}
	BasicShaderProgram(const std::string& vertexSource, const std::string& fragmentSource): ShaderProgram(vertexSource, fragmentSource){}
	~BasicShaderProgram() {

	}

	void setUniforms() override;


};
#endif // BASIC_SHADER_PROGRAM_H
