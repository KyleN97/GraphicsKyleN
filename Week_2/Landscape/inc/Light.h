#pragma once
//#include "gl_core_4_4.h"
#include <glm/glm.hpp>
class Light
{
public:
	Light(glm::vec4 pos, glm::vec3 col) {
		position = pos;
		colour = col;
		specColor = col;
	}
	~Light() {

	}
	glm::vec4 getPosition() {
		return position;
	}
	void SetAmbient(float amount) {
		ambientIntensity = amount;
	}
	void SetPosition(glm::vec4 pos) {
		position = pos;
	}
	void SetConeAngle(float angle) {
		coneAngle = angle;
	}
	void SetConeDirection(glm::vec3 pos) {
		coneDirection = pos;
	}
	void SetAttenuation(float amount) {
		attenuation = amount;
	}
	void SetSpecIntensity(float amount) {
		SpecIntensity = amount;
	}
	void SetAmbientIntensity(float amount) {
		ambientIntensity = amount;
	}
	float getSpecIntensity()
	{
		return SpecIntensity;
	}
	float getAmbientIntensity()
	{
		return ambientIntensity;
	}
	glm::vec3 getColour() {
		return colour;
	}

	glm::vec3 getSpecColor() {
		return specColor;
	}
	float getAttenuation() {
		return attenuation;
	}
	float getConeAngle() {
		return coneAngle;
	}
	glm::vec3 getConeDirection() {
		return coneDirection;
	}
	glm::vec3 colour;
	glm::vec3 specColor;
	glm::vec4 position;
	float SpecIntensity = 32.0f;
	float ambientIntensity = 1.0f;
private:
	float attenuation = 1.0f;
	float coneAngle = 0.0f;
	glm::vec3 coneDirection = glm::vec3(0, 0, -1);


};

