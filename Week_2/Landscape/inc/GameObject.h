#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <vector>
#include "Shader.h"
#include <Texture.h>
class Object
{
public:
	void Update(float d_time);//Update for each individaul object
public:
	glm::vec3 objectPosition = glm::vec3(0,0,0);
	float objectScale = 1.0f;
	glm::vec4 objectColor = glm::vec4(1,1,1,1);
	std::string objectType = "default_object";

	Shader* objectShader;//Shader for the object
	aie::Texture* objectTexture;//Texture for the object
	struct BufferData
	{
		unsigned int m_vbo, m_ibo, m_vao, m_IndicesCount;
	};
	struct Vertex
	{
		glm::vec4 pos;
		glm::vec2 uv;
	};
	//Buffer and Vertex data for the object
};
class GameObject : public Object
{

public:
	GameObject();
	~GameObject();
	void Create();
	void DrawUI();
	void CreateCube(glm::vec3 scale);
	void DrawCube(glm::mat4 projectionView);
	void DrawAll(glm::mat4 projectionView);

	std::vector<Object*> gameObjects;//All gameobjects

	//Creation of an object
	std::vector<Shader*> objectShaders;//All the object Shaders
	std::vector<aie::Texture*> objectTextures;//All the object Textures
	std::vector<BufferData*> objectData;//All the buffer data for the objects
private:

	int amountOfObjects = 0;//how many objects
};

