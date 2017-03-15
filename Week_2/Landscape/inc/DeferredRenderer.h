#pragma once
#include "Shader.h"
#include <gl_core_4_4.h>
class DeferredRenderer
{
public:
	DeferredRenderer();
	~DeferredRenderer();

	void CreateGPassBuffer(int windowWidth, int windowHeight); 
	void CreateLightBuffer(int windowWidth, int windowHeight);
private:
	Shader* shader;
	GLuint m_gpassFBO;
	GLuint m_albedoTexture;
	GLuint m_positionTexture;
	GLuint m_normalTexture;
	GLuint m_gpassDepth;
	GLuint m_lightFBO;
	GLuint m_lightTexture;
	bool status;

};

