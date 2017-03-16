#pragma once
#include <glm/glm.hpp>
#include <vector>
class Light
{
public:
	glm::vec4 getPosition() {
		return this->position;
	}
	int getLightType()
	{
		return this->typeOfLight;
	}
	int SetLightType(int type)
	{
		this->typeOfLight = type;
		return this->typeOfLight;
	}
	void SetAmbient(float amount) {
		this->ambientIntensity = amount;
	}
	void SetColor(glm::vec3 col) {
		this->colour = col;
	}
	void SetSpecColor(glm::vec3 col) {
		this->specColor = col;
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
	float SpecIntensity = 32.0f;
	float ambientIntensity = 1.0f;
	glm::vec3 colour;
	glm::vec3 specColor;
private:
	float attenuation = 1.0f;
	float coneAngle = 0.0f;
	glm::vec3 coneDirection = glm::vec3(0, 0, -1);

	glm::vec4 position;
	int typeOfLight = 0;

};
class LightManager : public Light
{
public:
	LightManager() {
		
	}
	~LightManager() {

	}
	void CreateLight(glm::vec4 pos, glm::vec3 col, int type) {
		worldLights.push_back(new Light());
		worldLights[amountOfLights]->SetPosition(pos);
		worldLights[amountOfLights]->SetColor(col);
		worldLights[amountOfLights]->SetSpecColor(col);
		worldLights[amountOfLights]->SetLightType(type);
		amountOfLights++;
	}
	void SetLightArrays(std::vector <Light*> worldLights) {
		GLspecCol = new glm::vec3[worldLights.size()];//Specular Colour of the light
		GLpos = new glm::vec4[worldLights.size()];//Position of the light
		GLcol = new glm::vec3[worldLights.size()];//Colour of the light
		GLconeDir = new glm::vec3[worldLights.size()];//Cone Direction of the spot light
		GLambient = new GLfloat[worldLights.size()];//Ambient of the light
		GLattenuation = new GLfloat[worldLights.size()];//Attenuation of the light
		GLconeangle = new GLfloat[worldLights.size()];//Angle of the cone for spot light
		GLspecPower = new GLfloat[worldLights.size()];//specular 												 //power of the light
		GLtypeOfLight = new GLint[worldLights.size()];//type of  light
		for (int i = 0; i < worldLights.size(); i++)
		{
			GLambient[i] = worldLights[i]->getAmbientIntensity();
			GLattenuation[i] = worldLights[i]->getAttenuation();
			GLconeangle[i] = worldLights[i]->getConeAngle();
			GLspecPower[i] = worldLights[i]->getSpecIntensity();
			GLspecCol[i] = worldLights[i]->getSpecColor();
			GLpos[i] = worldLights[i]->getPosition();
			GLcol[i] = worldLights[i]->getColour();
			GLconeDir[i] = worldLights[i]->getConeDirection();
			GLtypeOfLight[i] = worldLights[i]->getLightType();
			//Setting all the arrays with the lights data
		}
	}
	GLfloat *GLambient, *GLattenuation, *GLconeangle, *GLspecPower;
	glm::vec3 *GLspecCol, *GLcol, *GLconeDir;
	glm::vec4 *GLpos;
	GLint* GLtypeOfLight;
	std::vector<Light*> worldLights;
	int amountOfLights = 0;
};

