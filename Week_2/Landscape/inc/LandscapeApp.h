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
	Camera* m_camera;//Camera
	HeightMap* heightMap;//Heightmap
	PostProcessor* postProcessor;//Post Processor
	GameObject* ObjectCreator;//Object Creator
	std::vector<FBXGameObject*> gameModels;//Game Models
	std::vector<ParticleEmitter*> m_emitter;//Particles
	std::vector<Light*> lightSources;//Lights

	glm::vec3 m_positions[2];
	glm::quat m_rotations[2];
	//Slerping an object usign quarts

	//culling test
	glm::vec3 cullingObjectPosition = glm::vec3(5,5,5);
	float cullingObjectRadius = 1.5;

	//Wether we are rendering wireframe or not
	bool m_isWireframe = false;

};