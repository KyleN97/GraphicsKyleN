

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
	//Create a light at a certain position and colour -  push it into a vector of lights
	lightSources.push_back(new Light(glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(1, 1, 1)));
	//Create a gameModel and push it into a vector
	gameModels.push_back(new FBXGameObject("./models/pyro/pyro.fbx","Landscape/Shaders/fbxAnimatedShader",true));
	//Give this certain object a scale
	gameModels[0]->Scale(glm::vec3(0.001f, 0.001f, 0.001f));
	gameModels.push_back(new FBXGameObject("./models/soulspear/soulspear.fbx", "Landscape/Shaders/fbxShader",false));
	//Create an emitter and push it into a vector
	m_emitter.push_back(new ParticleEmitter("Landscape/Shaders/particleShader"));
	m_emitter[0]->Init(100000, 500, 0.1f, 1.0f, 1, 5, 1, 0.1f, glm::vec4(1, 1, 0, 1), glm::vec4(0, 0, 0, 1), glm::vec3(2, 2, 2));
	//Create an Object Creator
	ObjectCreator = new GameObject();
	//Create a Post Processor
	postProcessor = new PostProcessor();
	//Setting the background colour of the scene
	setBackgroundColour(0.25f, 0.25f, 0.25f);

	//---initialize gizmo primitive counts---
	Gizmos::create(10000, 10000, 10000, 10000);
	m_positions[0] = glm::vec3(10, 5, 10);
	m_positions[1] = glm::vec3(-10, 0, -10);
	m_rotations[0] = glm::quat(glm::vec3(0,-1, 0));
	m_rotations[1] = glm::quat(glm::vec3(0, 1, 0));
	//Setup the Fram Buffer and Quad for the window
	postProcessor->SetupFrameBuffer(getWindowHeight(),getWindowWidth());
	postProcessor->SetupFrameQuad  (getWindowHeight(),getWindowWidth());

	cubeShader = new Shader("Landscape/Shaders/cube");
	cubetex = new aie::Texture("Landscape/Textures/Tile.png");
	CreateCube();
	//---Create the heightmap---
	heightMap = new HeightMap();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	return true;
}

void LandscapeApp::shutdown() {
	delete m_camera;
	delete heightMap;
	delete postProcessor;
	delete ObjectCreator;
	Gizmos::destroy();
	for (auto& member : gameModels){
		delete member;
	}
	for (auto& member : m_emitter){
		delete member;
	}
	for (auto& member : lightSources) {
		delete member;
	}
	//delete all pointers, cleanup 
}

void LandscapeApp::update(float deltaTime) {
	Gizmos::clear();
	// query time since application started
	float time = getTime();
	static float wrap_width = 200.0f;

	//Turn on/off wireframe if enabled
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

	//Draw the Object Creator UI
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
	//Drawing UI

	//Draw the Post Process UI
	postProcessor->DrawPostProcessUI();
	//Update the camera
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
	//Call update and Draw for the emitters,models and the object creator

	float s = glm::cos(time) * 0.5f  + 0.5f;
	glm::vec3 p = (1.0f - s) * m_positions[0] + s * m_positions[1];
	glm::quat r = glm::slerp(m_rotations[0],m_rotations[1],s);

	gameModels[1]->SlerpTo(p, r);
	//Slerping a game model between two points
	const mat4 sphereMat = glm::translate(glm::vec3(vec3(glm::sin(time) * 3, 3, glm::cos(time) * 3)));//translate the sphere in an orbit 3 wide and 3 high

	Gizmos::addSphere(vec3(0, 0, 0), .5, 64, 12, vec4(1, 0, 0, 0.5f), &sphereMat);
	//Adding a sphere into the scene and rotation in a circle which will be the lights postion
	lightSources[0]->SetPosition(sphereMat[3].xyz);
	m_cameraPosition = m_camera->GetPos();
	//Setting the main lights position and storing the camera pos
	
	//Draw a Grid
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
	//Init the Post Processor
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
	//Draw the heightmap
	heightMap->DrawHeightMap(projectionView, lightSources, m_camera);

	for (auto& member : gameModels)
	{
		member->Draw(projectionView,lightSources,m_camera);
	}
	for (auto& member : m_emitter)
	{
		member->Draw(projectionView);
	}
	//Draw the game models and emitter

	// Ask openGL to use our shader program
	glUseProgram(cubeShader->m_program);

	glUniformMatrix4fv(glGetUniformLocation(cubeShader->m_program, "projectionView"), 1, false, glm::value_ptr(projectionView));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cubetex->getHandle());
	glUniform1i(glGetUniformLocation(cubeShader->m_program, "texture"), 0);

	// Bind VAO
	glBindVertexArray(cubeData.m_vao);

	// DRAW STUFF
	glDrawElements(GL_TRIANGLES, cubeData.m_IndicesCount, GL_UNSIGNED_BYTE, 0);

	// unbind the VAO, cleaning up after ourselves
	glBindVertexArray(0);

	// Deactivate the shader
	glUseProgram(0);


	//Draw the Post Processor
	postProcessor->DrawPostProcess(postProcessor->m_enablePostProcess, getWindowHeight(), getWindowWidth());
}

void LandscapeApp::CreateCube()
{
	Vertex verts[] = {

		// POSITION						COLOR
		// FRONT FACE				  - RED
		{glm::vec4(-0.5f,-0.5f, 0.5f, 1.0f),glm::vec2(0.0f, 0.0f)},	// 0
		{glm::vec4(0.5f,-0.5f, 0.5f, 1.0f), glm::vec2(0.0f, 1.0f)},	// 1
		{glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), glm::vec2(1.0f, 1.0f)},	// 2
		{glm::vec4(-0.5f, 0.5f, 0.5f, 1.0f),glm::vec2(1.0f, 0.0f)},
		
		{glm::vec4(-.5f, -.5f, -.5f, 1),    glm::vec2(1,0)},
		{glm::vec4(-.5f, .5f, -.5f, 1),     glm::vec2(1,1)},
		{glm::vec4(.5f, .5f, -.5f, 1),      glm::vec2(0,1)},// 3
		{glm::vec4(.5f, -.5f, -.5f, 1),     glm::vec2(0,0)},// 3

//{glm::vec4(-0.5f,-0.5f, 0.5f, 1.0f),glm::vec2(0.0f, 0.0f)},	// 0
//{ glm::vec4(0.5f,-0.5f, 0.5f, 1.0f), glm::vec2(0.0f, 1.0f) },	// 1
//{ glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), glm::vec2(1.0f, 1.0f) },	// 2
//{ glm::vec4(-0.5f, 0.5f, 0.5f, 1.0f),glm::vec2(1.0f, 0.0f) },
//
//{ glm::vec4(-.5f, -.5f, -.5f, 1),    glm::vec2(1,0) },
//{ glm::vec4(-.5f, .5f, -.5f, 1),     glm::vec2(1,1) },
//{ glm::vec4(.5f, .5f, -.5f, 1),      glm::vec2(0,1) },// 3
//{ glm::vec4(.5f, -.5f, -.5f, 1),     glm::vec2(0,0) },// 3
//
//{glm::vec4(-0.5f,-0.5f, 0.5f, 1.0f),glm::vec2(0.0f, 0.0f)},	// 0
//{ glm::vec4(0.5f,-0.5f, 0.5f, 1.0f), glm::vec2(0.0f, 1.0f) },	// 1
//{ glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), glm::vec2(1.0f, 1.0f) },	// 2
//{ glm::vec4(-0.5f, 0.5f, 0.5f, 1.0f),glm::vec2(1.0f, 0.0f) },
//
//{ glm::vec4(-.5f, -.5f, -.5f, 1),    glm::vec2(1,0) },
//{ glm::vec4(-.5f, .5f, -.5f, 1),     glm::vec2(1,1) },
//{ glm::vec4(.5f, .5f, -.5f, 1),      glm::vec2(0,1) },// 3
//{ glm::vec4(.5f, -.5f, -.5f, 1),     glm::vec2(0,0) }// 3
								  // 3
	};
	
	unsigned char indices[] = {
		0,2,1,  0,3,2,
		4,3,0,  4,7,3,
		4,1,5,  4,0,1,
		3,6,2,  3,7,6,
		1,6,5,  1,2,6,
		7,5,6,  7,4,5
	};
	
	cubeData.m_IndicesCount = sizeof(indices) / sizeof(unsigned char);
	
	// Generate the VAO and Bind bind it.
	// Our VBO (vertex buffer object) and IBO (Index Buffer Object) will be "grouped" with this VAO
	// other settings will also be grouped with the VAO. this is used so we can reduce draw calls in the render method.
	glGenVertexArrays(1, &cubeData.m_vao);
	glBindVertexArray(cubeData.m_vao);
	
	// Create our VBO and IBO.
	// Then tell Opengl what type of buffer they are used for
	// VBO a buffer in graphics memory to contains our vertices
	// IBO a buffer in graphics memory to contain our indices.
	// Then Fill the buffers with our generated data.
	// This is taking our verts and indices from ram, and sending them to the graphics card
	glGenBuffers(1, &cubeData.m_vbo);
	glGenBuffers(1, &cubeData.m_ibo);
	
	glBindBuffer(GL_ARRAY_BUFFER, cubeData.m_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeData.m_ibo);
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 4));
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

/*void LandscapeApp::DestroyCube()
{
// When We're Done, destroy the geometry
glDeleteBuffers(1, &m_Ibo);
glDeleteBuffers(1, &m_Vbo);
glDeleteVertexArrays(1, &m_Vao);
}*/
