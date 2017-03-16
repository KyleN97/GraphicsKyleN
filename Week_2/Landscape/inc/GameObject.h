#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <vector>
#include "Shader.h"
#include "Light.h"
#include "Camera.h"
#include <Texture.h>
class Object
{
public:
	void Update(float d_time);//Update for each individaul object
public:
	glm::vec3 objectPosition = glm::vec3(0,0,0);//objects position
	float objectScale = 1.0f;//objects scale
	glm::vec4 objectColor = glm::vec4(1,1,1,1);//objects color
	std::string objectType = "default_object";//objects default type

	Shader* objectShader;//Shader for the object
	aie::Texture* objectTexture;//Texture for the object
	struct BufferData
	{
		unsigned int m_vbo, m_ibo, m_vao, m_IndicesCount;
	};//vertex,index buffer + array object + indices count of the object
	struct Vertex
	{
		glm::vec4 pos;
		glm::vec2 uv;
	};//Buffer and Vertex data for the object
};
class GameObject : public Object
{

public:
	GameObject();
	~GameObject();
	void Create();//Create an object
	void DrawUI();//Draw the ui for object creation
	void CreateCube(glm::vec3 scale);//Create a cube
	//void CreateIsosahedron(glm::vec3 scale);
	//void DrawISO(glm::mat4 projectionView, std::vector<Light*> lightSources, Camera * m_camera);
	void DrawCube(glm::mat4 projectionView,std::vector<Light*> lightSources,Camera* m_camera);//Draw a cube
	void DrawAll(glm::mat4 projectionView, std::vector<Light*> lightSources, Camera* m_camera);//Draw all objects

	std::vector<Object*> gameObjects;//All gameobjects

	//Creation of an object
	std::vector<Shader*> objectShaders;//All the object Shaders
	//Shader* tessellationControlShader;
	//Shader* tessellationEvaluationShader;
	std::vector<aie::Texture*> objectTextures;//All the object Textures
	std::vector<BufferData*> objectData;//All the buffer data for the objects
private:

	int amountOfObjects = 0;//how many objects
};

