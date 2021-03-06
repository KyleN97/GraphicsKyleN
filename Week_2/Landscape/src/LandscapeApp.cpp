

/*
-------------------------------------------------------------------------------
INSTRUCTIONS:
-------------------------------------------------------------------------------
STEP 1: Load a shader program
See the LoadShader method

STEP 2: Generate Geometry
See the CreateGeometry method

STEP 3: Each Frame - Render Geometry (using the shader program)
See the DrawGeometry method

STEP 4: Unload Shader and Geometry
-------------------------------------------------------------------------------
*/
#define GLM_SWIZZLE
#include "LandscapeApp.h"
#include "Gizmos.h"
#include <imgui.h>
#include "Input.h"
#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "gl_core_4_4.h"
#include "Shader.h"
#include "Light.h"
#include "ParticleEmitter.h"
#include "PostProcessor.h"
#include "GameObject.h"
#include "FBXGameObject.h"
#include "HeightMap.h"
#include <iostream>
using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;
LandscapeApp::LandscapeApp() {
	
}

LandscapeApp::~LandscapeApp() {

}

bool LandscapeApp::startup() {
	float atPercent = 0.0f;//For loading percent display
	std::cout << "Loading... - " << atPercent * 10 << std::endl;

	//---Setup camera starting position and where it's looking---
	m_camera = new Camera(this);
	m_camera->SetPosition(glm::vec3(5.0f, 5.0f, 5.0f));
	m_camera->LookAt(glm::vec3(0.0f, 0.0f, 0.0f));
	atPercent++;
	std::cout << "Loading Camera... - " << atPercent * 10 << std::endl;
	//Create a light at a certain position and colour -  push it into a vector of lights
	gameLightManager = new LightManager();//Create a game light manager
	//---Point Light---//
	gameLightManager->CreateLight(glm::vec4(0.0f, 5.0f, 0.0f, 1.0f), glm::vec3(1, 1, 1), 1);
	gameLightManager->worldLights[0]->SetAmbientIntensity(0.5f);
	gameLightManager->SetAttenuation(1.0f);

	atPercent++;
	std::cout << "Loading Light Manager + Point Light... - " << atPercent * 10 << std::endl;

	//---Directional Light---//
	gameLightManager->CreateLight(glm::vec4(0.0f, 10.0f, 0.0f, 1.0f), glm::vec3(1, 1, 1), 0);
	gameLightManager->worldLights[1]->SetAttenuation(1.0f);

	atPercent++;
	std::cout << "Loading Directional Light... - " << atPercent * 10 << std::endl;

	//---Spot Light---//
	gameLightManager->CreateLight(glm::vec4(0.0f, 30.0f, 0.0f, 1.0f), glm::vec3(0, 1, 0), 2);
	gameLightManager->worldLights[2]->SetAttenuation(0.1f);
	gameLightManager->worldLights[2]->SetAmbient(0.5f);
	gameLightManager->worldLights[2]->SetConeAngle(15.0f);
	gameLightManager->worldLights[2]->SetConeDirection(glm::vec3(0, 0, -1));

	atPercent++;
	std::cout << "Loading Spot Light.. - " << atPercent * 10 << std::endl;

	//Create a gameModel and push it into a vector
	gameModels.push_back(new FBXGameObject("Landscape/models/pyro/pyro.fbx","Landscape/Shaders/fbxAnimatedShader",true));
	gameModels[0]->Scale(glm::vec3(0.001f, 0.001f, 0.001f));//Give this certain object a scale
	gameModels[0]->Translate(glm::vec3(0, 1, 0));//Moves this object to a certain position

	atPercent++;
	std::cout << "Loading Pyro... - " << atPercent * 10 << std::endl;
		
	gameModels.push_back(new FBXGameObject("Landscape/models/soulspear/soulspear.fbx", "Landscape/Shaders/fbxShader", false));

	atPercent++;
	std::cout << "Loading Soulspear... - " << atPercent * 10 << std::endl;

	//Create an emitter and push it into a vector
	m_emitter.push_back(new ParticleEmitter("Landscape/Shaders/particleShader"));
	m_emitter[0]->Init(100000, 500, 0.1f, 1.0f, 1, 5, 1, 0.1f, glm::vec4(1, 1, 0, 1), glm::vec4(0, 0, 0, 1), glm::vec3(10, 2, 2));

	atPercent++;
	std::cout << "Loading Particle System... - " << atPercent * 10 << std::endl;

	ObjectCreator = new GameObject();//Create an Object Creator

	atPercent++;
	std::cout << "Loading Object Creator... - " << atPercent * 10 << std::endl;
	
	postProcessor = new PostProcessor();//Create a Post Processor
	
	//Setup the Frame Buffer and Quad for the window
	postProcessor->SetupFrameBuffer(getWindowHeight(), getWindowWidth());
	postProcessor->SetupFrameQuad(getWindowHeight(), getWindowWidth());
	atPercent++;
	std::cout << "Loading Post Processor... - " << atPercent * 10 << std::endl;

	//Setting the background colour of the scene
	setBackgroundColour(0.25f, 0.25f, 0.25f);

	//---initialize gizmo primitive counts---
	Gizmos::create(10000, 10000, 10000, 10000);

	m_positions[0] = glm::vec3(10, 5, 10);
	m_positions[1] = glm::vec3(-10, 0, -10);
	m_rotations[0] = glm::quat(glm::vec3(0,-1, 0));
	m_rotations[1] = glm::quat(glm::vec3(0, 1, 0));
	//Setup default postion and rotation for the obejct using quarts

	heightMap = new HeightMap();//---Create the heightmap---
	atPercent++;
	std::cout << "Loading Heightmap... - " << atPercent * 10 << std::endl;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	std::cout << "Succesfully Loaded Engine\n";
	return true;
}

void LandscapeApp::shutdown() {
	delete m_camera;
	delete heightMap;
	delete postProcessor;
	delete ObjectCreator;
	delete gameLightManager;
	Gizmos::destroy();
	for (auto& member : gameModels){
		delete member;
	}
	for (auto& member : m_emitter){
		delete member;
	}
	//delete all pointers, cleanup 
}

void LandscapeApp::update(float deltaTime) {
	Gizmos::clear();

	// query time since application started
	float time = getTime();
	heightMap->timePassed = time;

	m_camera->Update(deltaTime);//Update the camera
	
	if (m_isWireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//Turn on/off wireframe if enabled

	for (int i = 0; i < gameLightManager->worldLights.size(); ++i)
	{
		std::string lightEditor;
		switch (i)
		{
		case 0:
			lightEditor = "Point Light Editor";
			break;
		case 1:
			lightEditor = "Directional Light Editor";
			break;
		case 2:
			lightEditor = "Spot Light Editor";
			break;
		}
		ImGui::Begin(lightEditor.c_str());
		ImGui::SliderFloat("Ambient Strength", &gameLightManager->worldLights[i]->ambientIntensity, 0, 10);
		ImGui::SliderFloat("Specular Strength", &gameLightManager->worldLights[i]->SpecIntensity, 0, 10000);
		ImGui::ColorEdit3("Light Color", glm::value_ptr(gameLightManager->worldLights[i]->colour));
		ImGui::ColorEdit3("Spec Light Color", glm::value_ptr(gameLightManager->worldLights[i]->specColor));
		ImGui::End();
	}//Draw the light UI/Editor for each light

	
	ImGui::Begin("Landscape Editor");
	ImGui::Checkbox("WireFrame", &m_isWireframe);
	ImGui::End();
	//Draw the Wireframe toggle UI

	//Draw the Object Creator UI
	ObjectCreator->DrawUI();
	for (auto& member : m_emitter){
		member->DrawUI();
	}

	ImGui::Begin("Debugger");
	std::string frameRatestr = "Average Framerate " + std::to_string(1.0f / deltaTime);
	ImGui::Text(frameRatestr.c_str());
	ImGui::Text("Camera Position");
	ImGui::Text(glm::to_string(m_camera->GetPos()).c_str());
	ImGui::Text("Light Position");
	ImGui::Text(glm::to_string(gameLightManager->worldLights[0]->getPosition()).c_str());
	ImGui::Text(glm::to_string(gameLightManager->worldLights[1]->getPosition()).c_str());
	ImGui::Text(glm::to_string(gameLightManager->worldLights[2]->getPosition()).c_str());

	ImGui::End();
	//Drawing UI

	//Draw the Post Process UI
	postProcessor->DrawPostProcessUI();
	gameLightManager->worldLights[2]->SetPosition(glm::vec4(m_camera->GetPos(),1));
	gameLightManager->worldLights[2]->SetConeDirection(m_camera->m_cameraLook);


	for (auto& member : m_emitter){
		member->Update(deltaTime, m_camera->GetView(), m_camera->GetPos());
	}//Update each emmiter
	for (auto& element :gameModels){
		element->Update(deltaTime, getTime());
		element->DrawUI(deltaTime);
	}//Update each game model
	for (auto& element : ObjectCreator->gameObjects){
		element->Update(deltaTime);
	}//Update each gameobject

	float s = glm::cos(time) * 0.5f  + 0.5f;
	glm::vec3 p = (1.0f - s) * m_positions[0] + s * m_positions[1];
	glm::quat r = glm::slerp(m_rotations[0],m_rotations[1],s);
	gameModels[1]->SlerpTo(p, r);
	//Slerping a game model between two points
	const mat4 sphereMat = glm::translate(glm::vec3(vec3(glm::cos(time * 0.5) * 30, 25, glm::sin(time * 0.5) * 30)));//translate the sphere in an orbit 3 wide and 3 high
	
	Gizmos::addSphere(cullingObjectPosition,cullingObjectRadius, 32, 32, glm::vec4(0, 1, 1, 1));//culling test object
	Gizmos::addSphere(vec3(0, 0, 0), .5, 64, 12, vec4(1, 0, 0, 0.5f), &sphereMat);
	
	//Adding a sphere into the scene and rotation in a circle which will be the lights postion
	gameLightManager->worldLights[0]->SetPosition(glm::vec4(sphereMat[3].xyz,1));

	//Draw a Grid
	//DrawGrid();
	
	// quit if we press escape
	aie::Input* input = aie::Input::getInstance();

	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();
}

void LandscapeApp::DrawGrid()
{
	// draw a simple grid with gizmos
	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	for (int i = 0; i < 21; ++i) {
		Gizmos::addLine(vec3(-10 + i, 0, 10),
			vec3(-10 + i, 0, -10),
			i == 10 ? white : black);
		Gizmos::addLine(vec3(10, 0, -10 + i),
			vec3(-10, 0, -10 + i),
			i == 10 ? white : black);
	}
}

void LandscapeApp::draw() {
	postProcessor->InitDrawPostProcess(postProcessor->m_enablePostProcess, getWindowWidth(), getWindowHeight());//Init the Post Processor
	
	clearScreen();// wipe the screen to the background colour

	glPolygonMode(GL_FRONT_AND_BACK,GL_FRONT);

	Gizmos::draw(m_camera->GetProjection() * m_camera->GetView());// update perspective in case window resized
	
	glm::mat4 projectionView = m_camera->GetProjection() * m_camera->GetView();//Setting up the projection view
	
	gameLightManager->SetLightArrays(gameLightManager->worldLights);//Setting light array values for shader passing


	heightMap->DrawHeightMap(projectionView, m_camera, gameLightManager);//Draw the heightmap

	for (auto& member : gameModels)
	{
		member->Draw(projectionView,m_camera,gameLightManager);
	}//Draw all gamemodels
	for (auto& member : m_emitter)
	{
		member->Draw(projectionView);
	}//Draw all emmitters

	ObjectCreator->DrawAll(projectionView, gameLightManager->worldLights,m_camera);//Draw all objects

	ImGui::Begin("Frustrum Culling");

	bool vis = m_camera->getFrustrumPlanes(projectionView, cullingObjectPosition.x,cullingObjectPosition.y,cullingObjectPosition.z,cullingObjectRadius);
	ImGui::Checkbox("Is culling object Visible: ", &vis);
	ImGui::End();

	postProcessor->DrawPostProcess(postProcessor->m_enablePostProcess, getWindowWidth(), getWindowHeight());//Draw the Post Processor
}