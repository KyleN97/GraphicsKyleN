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
class DeferredRenderer;
class LandscapeApp : public aie::Application {
public:

	LandscapeApp();

	virtual ~LandscapeApp();

	virtual bool startup();//On startup

	virtual void shutdown();//Omn shutdown

	virtual void update(float deltaTime);//On update
	
	virtual void draw();//On draw

	void DrawGrid();//Draw a grid within the world

protected:
	Camera* m_camera;//Camera
	HeightMap* heightMap;//Heightmap
	PostProcessor* postProcessor;//Post Processor
	GameObject* ObjectCreator;//Object Creator
	DeferredRenderer* d_Renderer;//Deffered Renderer
	std::vector<FBXGameObject*> gameModels;//Game Models
	std::vector<ParticleEmitter*> m_emitter;//Particles
	std::vector<Light*> lightSources;//Lights

	glm::vec3 m_positions[2];
	glm::quat m_rotations[2];
	//Slerping an object using quarts

	//culling test
	glm::vec3 cullingObjectPosition = glm::vec3(5,10,5);
	float cullingObjectRadius = 1.5;

	//Wether we are rendering wireframe or not
	bool m_isWireframe = false;

};