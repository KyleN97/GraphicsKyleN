#pragma once

#include "Application.h"
#include <glm/glm.hpp>
#include <vector>
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

	/*void LoadShader();*/
	/*void UnloadShader();*/

	/*void CreateCube();*/
	/*void DestroyCube();*/

	void CreateObject(glm::vec3 position, float scale);

	void CreateLandscape();
	void DestroyLandscape();

	void DrawLandscape();

protected:
	Camera* m_camera;
	Shader* shader;
	aie::Texture* m_texture;

	aie::Texture* m_grass;
	aie::Texture* m_sand;
	aie::Texture* m_snow;
	aie::Texture* m_rock;
	aie::Texture* m_splat;


	aie::Texture* m_heightMap;

	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;

	//unsigned int m_shader;
	bool m_isWireframe = false;
	unsigned int m_vertCount;
	unsigned int m_IndicesCount;

	unsigned int m_Vao;
	unsigned int m_Vbo;
	unsigned int m_Ibo;

	int M_LAND_WIDTH = 512, M_LAND_DEPTH = 512;
	const float m_vertSeperation = 0.1f;
	const float m_maxHeight = 2;

	glm::vec3 m_lightPosition;
	glm::vec3 m_cameraPosition;
	glm::vec3 m_lightColor;
	float m_lightAmbientStrength;
	glm::vec3 m_lightSpecColor = glm::vec3(1.0f,0.0f,0.0f);
	std::vector<glm::vec3> objectPosition;
	std::vector<float> objectScale;
	std::vector<bool> createObject;
	int amountOfObjects = 0;
	float m_specPower = 32.0f;
	struct Vertex
	{
		glm::vec4 pos;
		glm::vec2 uv;
		glm::vec4 vNormal;
		static void SetupVertexAttribPointers();
	};

};