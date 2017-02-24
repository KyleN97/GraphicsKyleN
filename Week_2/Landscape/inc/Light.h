#pragma once
//#include "gl_core_4_4.h"
#include <glm/glm.hpp>
class Light
{
public:
	Light(glm::vec3 pos, glm::vec3 col) {
		this->position = pos;
		this->colour = col;

	}
	Light(glm::vec3 pos, glm::vec3 col,glm::vec3 atten) {
		this->position = pos;
		this->colour = col;
		this->attenuation = atten;

	}
	~Light() {

	}
	glm::vec3 getPosition() {
		return position;
	}
	void SetPosition(glm::vec3 pos) {
		this->position = pos;
	}

	glm::vec3 getColour() {
		return colour;
	}
	glm::vec3 getAttenuation() {
		return attenuation;
	}

	glm::vec3 colour;

private:
	glm::vec3 position;
	glm::vec3 attenuation = glm::vec3(1,0,0);

};

