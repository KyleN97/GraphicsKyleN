#pragma once
#include "Shader.h"
#include "Camera.h"
#include <gl_core_4_4.h>
class DeferredRenderer
{
public:
	DeferredRenderer();
	~DeferredRenderer();

	void CreateGPassBuffer(int windowWidth, int windowHeight); 
	void CreateLightBuffer(int windowWidth, int windowHeight);
	void Render(Camera* m_camera);
	void drawDirectionalLight(const glm::vec3& direction, const glm::vec3& diffuse,Camera* m_camera);
private:
	Shader* gBufferShader;
	Shader* compShader;
	Shader* directLight;
	GLuint m_gpassFBO;
	GLuint m_albedoTexture;
	GLuint m_positionTexture;
	GLuint m_normalTexture;
	GLuint m_gpassDepth;
	GLuint m_lightFBO;
	GLuint m_lightTexture;
	GLenum status;

};

