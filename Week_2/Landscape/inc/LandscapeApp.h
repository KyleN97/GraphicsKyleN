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
class Light;
class Camera;
class ParticleEmitter;
class LandscapeApp : public aie::Application {
public:

	LandscapeApp() {}
	virtual ~LandscapeApp() {}

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

	void DrawGrid();

	/*void LoadShader();*/
	/*void UnloadShader();*/

	/*void CreateCube();*/
	/*void DestroyCube();*/

	void DrawAABBFilled();
	void DrawSphere();
	void DrawRing();
	void CreateLandscape();
	void DestroyLandscape();

	void DrawLandscape();

	void CreateFBXOpenGLBuffers(FBXFile *file);
	void CreateFBXAnimatedOpenGLBuffers(FBXFile *fbx);

	void CleanupFBXOpenGLBuffers(FBXFile *file);

	void SetupFrameBuffer();
	void SetupFrameQuad();
	void PlayAnimationTo(int a,int b);
	void InitDrawPostProcess(bool isOn);
	void DrawPostProcess(bool isOn);

protected:
	Camera* m_camera;
	Shader* shader;
	Shader* fbxShader;
	Shader* animatedFBXShader;
	Shader* particleShader;
	Shader* frameBufferShader;
	ParticleEmitter* m_emitter;

	std::vector<Light*> lightSources;

	glm::vec3 m_positions[2];
	glm::quat m_rotations[2];

	FBXFile *m_myFbxModel;
	FBXFile* m_animatedFBXmodel;
	FBXSkeleton* skeleton;
	FBXAnimation* animation;
	glm::mat4 fbxMat;
	glm::mat4 fbxAnimationMat;
	float fbxScale = 1.0f;
	float fbxFrameCount;
	float fbxCurrentFrame = 0;
	aie::Texture* m_texture;
	aie::Texture* m_grass;
	aie::Texture* m_sand;
	aie::Texture* m_snow;
	aie::Texture* m_rock;
	aie::Texture* m_splat;
	aie::Texture* m_heightMap;

	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;
	bool showBones = false;
	bool m_enablePostProcess = false;
	bool m_enableDistortion = true;
	bool m_enableBlur = false;
	bool m_enableGrey = false;
	bool m_isWireframe = false;
	unsigned int m_vertCount;
	unsigned int m_IndicesCount;

	unsigned int m_Vao;
	unsigned int m_Vbo;
	unsigned int m_Ibo;
	//Post
	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_fbo;
	unsigned int m_fboTexture;
	unsigned int m_fboDepth;
	//---
	int M_LAND_WIDTH = 512, M_LAND_DEPTH = 512;
	const float m_vertSeperation = 0.1f;
	const float m_maxHeight = 2;

	glm::vec3 m_cameraPosition;
	float m_lightAmbientStrength;
	int num_Lights = 0;

	//Object Creation
	std::vector<glm::vec3> objectPosition;
	std::vector<float> objectScale;
	std::vector<bool> createObject;
	std::vector<glm::vec4> objectColor;
	std::vector<std::string> objectType;
	int amountOfObjects = 1;
	//End Object Creation
	float m_specPower = 32.0f;
	struct Vertex
	{
		glm::vec4 pos;
		glm::vec2 uv;
		glm::vec4 vNormal;
		static void SetupVertexAttribPointers();
	};

};