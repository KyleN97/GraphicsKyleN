#pragma once

#include "Application.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>
#include <Texture.h>
class Shader;
class Light;
class Camera;
class ParticleEmitter;
class PostProcessor;
class GameObject;
class FBXGameObject;
class HeightMap;
class LandscapeApp : public aie::Application {
public:

	LandscapeApp();
	virtual ~LandscapeApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

	void DrawGrid();

	/*void DestroyCube();*/


protected:
	Camera* m_camera;
	HeightMap* heightMap;
	PostProcessor* postProcessor;
	GameObject* ObjectCreator;
	std::vector<FBXGameObject*> gameModels;
	std::vector<ParticleEmitter*> m_emitter;
	std::vector<Light*> lightSources;

	glm::vec3 m_positions[2];
	glm::quat m_rotations[2];

	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;

	bool m_isWireframe = false;


	glm::vec3 m_cameraPosition;


};