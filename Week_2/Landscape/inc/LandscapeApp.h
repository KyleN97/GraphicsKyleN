#pragma once

#include "Application.h"
#include <glm/glm.hpp>
class Camera;
namespace aie
{
	class Texture;
}
class Shader;
class LandscapeApp : public aie::Application {
public:

	LandscapeApp();
	virtual ~LandscapeApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

	void DrawGrid();

	void LoadShader();
	void UnloadShader();

	void CreateCube();
	void DestroyCube();

	void CreateLandscape();
	void DestroyLandscape();

	void DrawLandscape();

protected:
	Camera* m_camera;
	Shader* shader;
	aie::Texture* m_texture;
	aie::Texture* m_heightMap;

	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;

	//unsigned int m_shader;

	unsigned int m_vertCount;
	unsigned int m_IndicesCount;

	unsigned int m_Vao;
	unsigned int m_Vbo;
	unsigned int m_Ibo;

	const int M_LAND_WIDTH = 512, M_LAND_DEPTH = 512;
	const float m_vertSeperation = 0.1f;
	const float m_maxHeight = 3.0f;

	glm::vec3 m_lightPosition;
	glm::vec3 m_lightColor;
	float m_lightAmbientStrength;
	struct Vertex
	{
		glm::vec4 pos;
		glm::vec2 uv;
		glm::vec4 vNormal;
		static void SetupVertexAttribPointers();
	};

};