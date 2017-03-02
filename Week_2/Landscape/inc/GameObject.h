#pragma once
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vector>
class Object
{
public:
	void Draw();
	void Update(float d_time);
public:
	glm::vec3 objectPosition = glm::vec3(0,0,0);
	float objectScale = 1.0f;
	glm::vec4 objectColor = glm::vec4(1,1,1,1);
	std::string objectType = "default_object";

};
class GameObject : public Object
{

public:
	GameObject();
	~GameObject();
	void Create();
	void DrawUI();
	void DrawEditUI();
	std::vector<Object*> gameObjects;
private:
	int amountOfObjects = 0;
};

