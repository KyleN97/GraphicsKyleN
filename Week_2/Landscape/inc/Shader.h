#pragma once
#include <string>
#include "gl_core_4_4.h"


class Shader
{
public:
	void Bind();//Binds the shader
	Shader(const std::string& fileName);//Load in a shader from a file - this creates a program and sets it up for ise
	~Shader();
	GLuint m_program;//Program of each shader
private:
	static const unsigned int NUM_SHADER = 2;
	GLuint CreateShader(const std::string& text, GLenum shaderType);//Create the type of shaders passed through
	std::string LoadShader(const std::string& fileName);//Load the sctaul shaders
	void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage);//Check the shhaders for erros
	GLuint m_shaders[NUM_SHADER];//Shaders within application

};

