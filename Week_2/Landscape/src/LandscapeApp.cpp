

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
using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

LandscapeApp::LandscapeApp()
{
}

LandscapeApp::~LandscapeApp()
{
}

bool LandscapeApp::startup() {

	//---Setup camera starting position and where it's looking---
	m_camera = new Camera();
	m_camera->SetPosition(glm::vec3(5.0f, 5.0f, 5.0f));
	m_camera->LookAt(glm::vec3(0.0f, 0.0f, 0.0f));

	lightSources.push_back(new Light(glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(1, 1, 1)));

	gameModels.push_back(new FBXGameObject("./models/pyro/pyro.fbx","Landscape/Shaders/fbxAnimatedShader",true));
	gameModels[0]->Scale(glm::vec3(0.001f, 0.001f, 0.001f));
	gameModels.push_back(new FBXGameObject("./models/soulspear/soulspear.fbx", "Landscape/Shaders/fbxShader",false));

	//shader = new Shader("Landscape/Shaders/basicShader");

	m_emitter.push_back(new ParticleEmitter("Landscape/Shaders/particleShader"));
	m_emitter[0]->Init(100000, 500, 0.1f, 1.0f, 1, 5, 1, 0.1f, glm::vec4(1, 1, 0, 1), glm::vec4(0, 0, 0, 1), glm::vec3(2, 2, 2));

	ObjectCreator = new GameObject();
	postProcessor = new PostProcessor();
	setBackgroundColour(0.25f, 0.25f, 0.25f);

	//---initialize gizmo primitive counts---
	Gizmos::create(10000, 10000, 10000, 10000);
	m_positions[0] = glm::vec3(10, 5, 10);
	m_positions[1] = glm::vec3(-10, 0, -10);
	m_rotations[0] = glm::quat(glm::vec3(0,-1, 0));
	m_rotations[1] = glm::quat(glm::vec3(0, 1, 0));

	postProcessor->SetupFrameBuffer(getWindowHeight(),getWindowWidth());
	postProcessor->SetupFrameQuad  (getWindowHeight(),getWindowWidth());

	//---Create the landscape---
	heightMap = new HeightMap();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	return true;
}

void LandscapeApp::shutdown() {
	//DestroyLandscape();
	//delete shader;
	delete m_camera;
	Gizmos::destroy();
	for (auto& member : gameModels)
	{
		delete member;
	}
	
	//CleanupFBXOpenGLBuffers(m_myFbxModel);
	//m_myFbxModel->unload();
	//delete m_myFbxModel;
	//CleanupFBXOpenGLBuffers(m_animatedFBXmodel);
	//m_animatedFBXmodel->unload();
	//delete m_animatedFBXmodel;
}

void LandscapeApp::update(float deltaTime) {
	Gizmos::clear();
	// query time since application started
	float time = getTime();
	static float wrap_width = 200.0f;

	if (m_isWireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	//if (showBones)
	//{
	//	for (int i =0;i< skeleton->m_boneCount;i++)
	//	{
	//		//const glm::mat4 trans = skeleton->m_bones[i];
	//		//const glm::mat4 scaleDown = { glm::vec4(0.001, 0, 0, 0),
	//		//						glm::vec4(0, 0.001, 0, 0),
	//		//						glm::vec4(0, 0, 0.001, 0),
	//		//						glm::vec4(0, 0, 0, 1) };
	//		//const glm::mat4 finalTrans = trans * scaleDown;
	//		//
	//		//
	//		//
	//		//Gizmos::addSphere(glm::vec3(skeleton->m_bones[i][3][0], skeleton->m_bones[i][3][1],skeleton->m_bones[i][3][2]),2,6,6,glm::vec4(1,0,0,1), &finalTrans);
	//		
	//	}
	//}
	//else
	//{
	//	
	//}


	ImGui::Begin("Lighting Editor");
	ImGui::SliderFloat("Ambient Strength", &lightSources[0]->ambientIntensity, 0, 1);
	ImGui::SliderFloat("Specular Strength", &lightSources[0]->SpecIntensity, 0, 10000);
	ImGui::ColorEdit3("Light Color", glm::value_ptr(lightSources[0]->colour));
	ImGui::ColorEdit3("Spec Light Color", glm::value_ptr(lightSources[0]->specColor));
	ImGui::End();
	
	ImGui::Begin("Landscape Editor");
	ImGui::Checkbox("WireFrame", &m_isWireframe);
	ImGui::End();

	ObjectCreator->DrawUI();
	ObjectCreator->DrawEditUI();

	ImGui::Begin("Debugger");
	std::string frameRatestr = "Average Framerate " + std::to_string(1.0f / deltaTime);
	ImGui::Text(frameRatestr.c_str());
	ImGui::Text("Camera Position");
	ImGui::Text(glm::to_string(m_camera->GetPos()).c_str());
	ImGui::Text("Light Position");
	ImGui::Text(glm::to_string(lightSources[0]->getPosition()).c_str());
	ImGui::End();

	postProcessor->DrawPostProcessUI();

	m_camera->Update(deltaTime);


	for (auto& member : m_emitter){
		member->Update(deltaTime, m_camera->GetView(), m_camera->GetPos());
	}
	for (auto& element :gameModels){
		element->Update(deltaTime, getTime());
		element->DrawUI(deltaTime);
	}
	for (auto& element : ObjectCreator->gameObjects){
		element->Draw();
		element->Update(deltaTime);
	}

	const mat4 sphereMat = glm::translate(glm::vec3(vec3(glm::sin(time) * 3, 3, glm::cos(time) * 3)));//translate the sphere in an orbit 3 wide and 3 high

	float s = glm::cos(time) * 0.5f  + 0.5f;
	glm::vec3 p = (1.0f - s) * m_positions[0] + s * m_positions[1];
	glm::quat r = glm::slerp(m_rotations[0],m_rotations[1],s);

	gameModels[1]->SlerpTo(p, r);

	Gizmos::addSphere(vec3(0, 0, 0), .5, 64, 12, vec4(1, 0, 0, 0.5f), &sphereMat);

	lightSources[0]->SetPosition(sphereMat[3].xyz);
	m_cameraPosition = m_camera->GetPos();

	DrawGrid();

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
	
	postProcessor->InitDrawPostProcess(postProcessor->m_enablePostProcess, getWindowHeight(), getWindowWidth());
	// wipe the screen to the background colour
	clearScreen();
	glPolygonMode(GL_FRONT_AND_BACK,GL_FRONT);
	Gizmos::draw(m_projectionMatrix * m_camera->GetView());

	// update perspective in case window resized
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
										  getWindowWidth() / (float)getWindowHeight(),
										  0.1f, 1000.f);
	glm::mat4 projectionView = m_projectionMatrix * m_camera->GetView();

	heightMap->DrawHeightMap(projectionView, lightSources, m_camera);

	for (auto& member : gameModels)
	{
		member->Draw(projectionView,lightSources,m_camera);
	}
	for (auto& member : m_emitter)
	{
		member->Draw(projectionView);
	}

	postProcessor->DrawPostProcess(postProcessor->m_enablePostProcess, getWindowHeight(), getWindowWidth());
}

/*void LandscapeApp::CreateCube()
{
Vertex verts[] = {

// POSITION						COLOR
// FRONT FACE				  - RED
{glm::vec4(-0.5f,-0.5f, 0.5f, 1.0f),glm::vec4(1.0f, 0.0f, 0.0f, 0.5f)},	// 0
{glm::vec4(0.5f,-0.5f, 0.5f, 1.0f),glm::vec4(1.0f, 0.0f, 0.0f, 0.5f)},	// 1
{glm::vec4(0.5f, 0.5f, 0.5f, 1.0f),glm::vec4(1.0f, 0.0f, 0.0f, 0.5f)},	// 2
{glm::vec4(-0.5f, 0.5f, 0.5f, 1.0f),glm::vec4(1.0f, 0.0f, 0.0f, 0.5f)},	// 3
};

unsigned char indices[] = {
0, 1, 2,	0, 2, 3			// front facme
};

m_IndicesCount = sizeof(indices) / sizeof(unsigned char);

// Generate the VAO and Bind bind it.
// Our VBO (vertex buffer object) and IBO (Index Buffer Object) will be "grouped" with this VAO
// other settings will also be grouped with the VAO. this is used so we can reduce draw calls in the render method.
glGenVertexArrays(1, &m_Vao);
glBindVertexArray(m_Vao);

// Create our VBO and IBO.
// Then tell Opengl what type of buffer they are used for
// VBO a buffer in graphics memory to contains our vertices
// IBO a buffer in graphics memory to contain our indices.
// Then Fill the buffers with our generated data.
// This is taking our verts and indices from ram, and sending them to the graphics card
glGenBuffers(1, &m_Vbo);
glGenBuffers(1, &m_Ibo);

glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ibo);

glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

Vertex::SetupVertexAttribPointers();

glBindVertexArray(0);
glBindBuffer(GL_ARRAY_BUFFER, 0);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}*/

/*void LandscapeApp::DestroyCube()
{
// When We're Done, destroy the geometry
glDeleteBuffers(1, &m_Ibo);
glDeleteBuffers(1, &m_Vbo);
glDeleteVertexArrays(1, &m_Vao);
}*/
