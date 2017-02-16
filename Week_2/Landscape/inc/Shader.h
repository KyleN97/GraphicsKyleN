#pragma once
#include <string>
#include "gl_core_4_4.h"


class Shader
{
public:
	void Bind();
	Shader(const std::string& fileName);
	~Shader();
	GLuint m_program;
private:
	static const unsigned int NUM_SHADER = 2;
	GLuint CreateShader(const std::string& text, GLenum shaderType);
	std::string LoadShader(const std::string& fileName);
	void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage);
	GLuint m_shaders[NUM_SHADER];

};

