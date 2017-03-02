#pragma once

#include "Application.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <vector>
//#include "FBXFile.h"
//class GLMesh
//{
//public:
//	unsigned int vao;
//	unsigned int vbo;
//	unsigned int ibo;
//};
namespace aie
{
	class Texture;
}
class Shader;
class Light;
class Camera;
class ParticleEmitter;
class PostProcessor;
class GameObject;
class FBXGameObject;
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

	void CreateLandscape();
	void DestroyLandscape();

	void DrawLandscape();

protected:
	Camera* m_camera;
	Shader* shader;
	PostProcessor* postProcessor;
	GameObject* ObjectCreator;
	std::vector<FBXGameObject*> gameModels;
	std::vector<ParticleEmitter*> m_emitter;
	std::vector<Light*> lightSources;

	glm::vec3 m_positions[2];
	glm::quat m_rotations[2];

	aie::Texture* m_texture;
	aie::Texture* m_grass;
	aie::Texture* m_sand;
	aie::Texture* m_snow;
	aie::Texture* m_rock;
	aie::Texture* m_splat;
	aie::Texture* m_heightMap;

	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;
	//bool showBones = false;

	bool m_isWireframe = false;
	unsigned int m_vertCount;
	unsigned int m_IndicesCount;

	unsigned int m_Vao;
	unsigned int m_Vbo;
	unsigned int m_Ibo;

	int M_LAND_WIDTH = 512, M_LAND_DEPTH = 512;
	const float m_vertSeperation = 0.1f;
	const float m_maxHeight = 2;

	glm::vec3 m_cameraPosition;

	struct Vertex
	{
		glm::vec4 pos;
		glm::vec2 uv;
		glm::vec4 vNormal;
		static void SetupVertexAttribPointers();
	};

};