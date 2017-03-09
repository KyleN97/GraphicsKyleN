#pragma once
//#include "gl_core_4_4.h"
#include <glm/glm.hpp>
class Light
{
public:
	Light(glm::vec4 pos, glm::vec3 col) {
		this->position = pos;
		this->colour = col;
		this->specColor = col;
	}
	//Light(glm::vec3 pos, glm::vec3 col,glm::vec3 atten) {
	//	this->position = pos;
	//	this->colour = col;
	//	this->attenuation = atten;
	//
	//}
	~Light() {

	}
	glm::vec4 getPosition() {
		return this->position;
	}
	void SetPosition(glm::vec4 pos) {
		this->position = pos;
	}
	float getSpecIntensity()
	{
		return this->SpecIntensity;
	}
	float getAmbientIntensity()
	{
		return this->ambientIntensity;
	}
	glm::vec3 getColour() {
		return this->colour;
	}

	glm::vec3 getSpecColor() {
		return this->specColor;
	}
	float getAttenuation() {
		return this->attenuation;
	}
	float getConeAngle() {
		return this->coneAngle;
	}
	glm::vec3 getConeDirection() {
		return this->coneDirection;
	}
	glm::vec3 colour;
	glm::vec3 specColor;
	glm::vec4 position;
	float attenuation;
	float coneAngle;
	glm::vec3 coneDirection;
	float SpecIntensity = 32.0f;
	float ambientIntensity = 0.05f;
private:


};

