#pragma once
//#include "gl_core_4_4.h"
#include <glm/glm.hpp>
class Light
{
public:
	Light(glm::vec4 pos, glm::vec3 col,int type) {
		this->position = pos;
		this->colour = col;
		this->specColor = col;
		this->typeOfLight = type;
	}
	~Light() {

	}
	glm::vec4 getPosition() {
		return this->position;
	}
	int getLightType()
	{
		return this->typeOfLight;
	}
	int setLightType(int type)
	{
		this->typeOfLight = type;
	}
	void SetAmbient(float amount) {
		this->ambientIntensity = amount;
	}
	void SetPosition(glm::vec4 pos) {
		this->position = pos;
	}
	void SetConeAngle(float angle) {
		this->coneAngle = angle;
	}
	void SetConeDirection(glm::vec3 pos) {
		this->coneDirection = pos;
	}
	void SetAttenuation(float amount) {
		this->attenuation = amount;
	}
	void SetSpecIntensity(float amount) {
		this->SpecIntensity = amount;
	}
	void SetAmbientIntensity(float amount) {
		this->ambientIntensity = amount;
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
	float SpecIntensity = 32.0f;
	float ambientIntensity = 1.0f;
private:
	int typeOfLight = 0;
	float attenuation = 1.0f;
	float coneAngle = 0.0f;
	glm::vec3 coneDirection = glm::vec3(0, 0, -1);


};

