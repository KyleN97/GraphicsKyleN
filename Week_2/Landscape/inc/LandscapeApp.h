#pragma once

#include "Application.h"
#include <glm/glm.hpp>
#include <vector>
#include "FBXFile.h"
class GLMesh
{
public:
	unsigned int vao;
	unsigned int vbo;
	unsigned int ibo;
};
namespace aie
{
	class Texture;
}
class Shader;
class Camera;
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
	void DrawAABBFilled();
	void DrawSphere();
	void DrawRing();
	void DrawTextureObject();
	void LoadObjectShaders();
	void CreateLandscape();
	void DestroyLandscape();

	void DrawLandscape();

	void CreateFBXOpenGLBuffers(FBXFile *file);
	void CleanupFBXOpenGLBuffers(FBXFile *file);
	void LoadShaders();
	void UnloadShaders();

	void AddLight();

protected:
	Camera* m_camera;
	Shader* shader;
	Shader* fbxShader;
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

	std::vector< unsigned int> m_objectShaders;

	FBXFile *m_myFbxModel;

	int M_LAND_WIDTH = 512, M_LAND_DEPTH = 512;
	const float m_vertSeperation = 0.1f;
	const float m_maxHeight = 2;
	//World Lighting
	glm::vec3 m_lightPosition;
	glm::vec3 m_cameraPosition;
	glm::vec3 m_lightColor;
	float m_lightAmbientStrength;
	glm::vec3 m_lightSpecColor = glm::vec3(1.0f,0.0f,0.0f);
	//End World Lighting
	//Object Creation
	std::vector<glm::vec3> objectPosition;
	std::vector<float> objectScale;
	std::vector<bool> createObject;
	std::vector<aie::Texture*> objectTexture;
	std::vector<glm::vec4> objectColor;
	std::vector<std::string> objectType;
	//End Object Creation
	int amountOfObjects = 1;
	float m_specPower = 32.0f;
	struct Vertex
	{
		glm::vec4 pos;
		glm::vec2 uv;
		glm::vec4 vNormal;
		static void SetupVertexAttribPointers();
	};

};