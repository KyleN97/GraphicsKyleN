

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
#include <Texture.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "gl_core_4_4.h"
#include "Shader.h"
#include "Light.h"
#include "ParticleEmitter.h"
#include "PostProcessor.h"
#include "GameObject.h"
#include "FBXGameObject.h"
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

	#pragma region CameraSetup
	//---Setup camera starting position and where it's looking---
	m_camera = new Camera();
	m_camera->SetPosition(glm::vec3(5.0f, 5.0f, 5.0f));
	m_camera->LookAt(glm::vec3(0.0f, 0.0f, 0.0f));
	#pragma endregion
	#pragma region LightSetup
	//---setup light---
	lightSources.push_back(new Light(glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(1, 1, 1)));
	//m_lightAmbientStrength = 0.05f;
	#pragma endregion
	#pragma region TexturesSetup
	//---load tile---
	m_texture = new aie::Texture();
	m_texture->load("Landscape/Textures/Tile.png");
	//---load heightmap---
	m_heightMap = new aie::Texture();
	m_heightMap->load("Landscape/Textures/heightmap.bmp");
	//---load grass---
	m_grass = new aie::Texture();
	m_grass->load("Landscape/Textures/grass.png");
	//---load rock---
	m_rock = new aie::Texture();
	m_rock->load("Landscape/Textures/rock.png");
	//---load sand---
	m_sand = new aie::Texture();
	m_sand->load("Landscape/Textures/sand.png");
	//---load snow---
	m_snow = new aie::Texture();
	m_snow->load("Landscape/Textures/snow.png");
	//---load splat--
	m_splat = new aie::Texture();
	m_splat->load("Landscape/Textures/splat.jpg");
#pragma endregion
	#pragma region FBXSetup
	gameModels.push_back(new FBXGameObject("./models/pyro/pyro.fbx","Landscape/Shaders/fbxAnimatedShader",true));
	gameModels[0]->Scale(glm::vec3(0.001f, 0.001f, 0.001f));
	gameModels.push_back(new FBXGameObject("./models/soulspear/soulspear.fbx", "Landscape/Shaders/fbxShader",false));
#pragma endregion
	#pragma region ShaderSetup
	//fbxShader = new Shader("Landscape/Shaders/fbxShader");
	//animatedFBXShader = new Shader("Landscape/Shaders/fbxAnimatedShader");
	shader = new Shader("Landscape/Shaders/basicShader");
	particleShader = new Shader("Landscape/Shaders/particleShader");
	//frameBufferShader = new Shader("Landscape/Shaders/frameBufferShader");
#pragma endregion
	#pragma region ParticlesSetup
	m_emitter = new ParticleEmitter();
	m_emitter->Init(100000, 500, 0.1f, 1.0f, 1, 5, 1, 0.1f, glm::vec4(1, 1, 0, 1), glm::vec4(0, 0, 0, 1), glm::vec3(2, 2, 2));
	#pragma endregion
	#pragma region ObjectCreatorSetup
	ObjectCreator = new GameObject();
#pragma endregion
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
	CreateLandscape();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	return true;
}

void LandscapeApp::shutdown() {
	DestroyLandscape();
	delete shader;
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
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	}
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


	#pragma region Lighting GUI
	ImGui::Begin("Lighting Editor");
	ImGui::SliderFloat("Ambient Strength", &lightSources[0]->ambientIntensity, 0, 1);
	ImGui::SliderFloat("Specular Strength", &lightSources[0]->SpecIntensity, 0, 10000);
	ImGui::ColorEdit3("Light Color", glm::value_ptr(lightSources[0]->colour));
	ImGui::ColorEdit3("Spec Light Color", glm::value_ptr(lightSources[0]->specColor));
	ImGui::End();
	#pragma endregion
	
	#pragma region Landscape GUI
	ImGui::Begin("Landscape Editor");
	ImGui::Checkbox("WireFrame", &m_isWireframe);
	ImGui::End();
	#pragma endregion
	
	#pragma region ObjectGUI
	
	ObjectCreator->DrawUI();
	
	ObjectCreator->DrawEditUI();

#pragma endregion

	#pragma region DebuggerGUI
		ImGui::Begin("Debugger");
		std::string frameRatestr = "Average Framerate " + std::to_string(1.0f / deltaTime);
		ImGui::Text(frameRatestr.c_str());
		ImGui::Text("Camera Position");
		ImGui::Text(glm::to_string(m_camera->GetPos()).c_str());
		ImGui::Text("Light Position");
		ImGui::Text(glm::to_string(lightSources[0]->getPosition()).c_str());
		ImGui::End();
	#pragma endregion

	#pragma region PostProcess
	postProcessor->DrawPostProcessUI();
	#pragma endregion

	m_camera->Update(deltaTime);



	m_emitter->Update(deltaTime, m_camera->GetView(),m_camera->GetPos());
	for (auto& element :gameModels)
	{
		element->Update(deltaTime, getTime());
		element->DrawUI(deltaTime);
	}
	for (auto& element : ObjectCreator->gameObjects)
	{
		element->Draw();
		element->Update(deltaTime);
	}

	const mat4 sphereMat = glm::translate(glm::vec3(vec3(glm::sin(time) * 3, 3, glm::cos(time) * 3)));//translate the sphere in an orbit 3 wide and 3 high

	float s = glm::cos(time) * 0.5f  + 0.5f;
	glm::vec3 p = (1.0f - s) * m_positions[0] + s * m_positions[1];
	glm::quat r = glm::slerp(m_rotations[0],m_rotations[1],s);

	//gameModels[0]->Translate(glm::vec3(glm::vec3(glm::sin(time) * 3, 3, glm::cos(time) * 3)));
	//gameModels[0]->Scale(glm::vec3(0.001, 0.001, 0.001));
	gameModels[1]->SlerpTo(p, r);

	Gizmos::addSphere(vec3(0, 0, 0), .5, 64, 12, vec4(1, 0, 0, 0.5f), &sphereMat);

	lightSources[0]->SetPosition(sphereMat[3].xyz);
	m_cameraPosition = m_camera->GetPos();

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

	#pragma region BindTextures
	//setup texture in open gl - select the first texture as active, then bind it 
	//also set it up as a uniform variable for shader
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture->getHandle());
	glUniform1i(glGetUniformLocation(shader->m_program, "texture"), 0);

	//setup grass texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_grass->getHandle());
	glUniform1i(glGetUniformLocation(shader->m_program, "grass"), 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_rock->getHandle());
	glUniform1i(glGetUniformLocation(shader->m_program, "rock"), 2);


	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_sand->getHandle());
	glUniform1i(glGetUniformLocation(shader->m_program, "sand"), 3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_snow->getHandle());
	glUniform1i(glGetUniformLocation(shader->m_program, "snow"), 4);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, m_splat->getHandle());
	glUniform1i(glGetUniformLocation(shader->m_program, "splat"), 5);
	#pragma endregion

	#pragma region Landscape

	// STEP 1: enable the shader program for rendering
	shader->Bind();

	// Step 2: send uniform variables to the shader
	glUniformMatrix4fv(
		glGetUniformLocation(shader->m_program, "projectionView"),
		1,
		false,
		glm::value_ptr(projectionView));
	// Step 3: Bind the VAO
	glUniform1f(glGetUniformLocation(shader->m_program, "lightAmbientStrength"), lightSources[0]->getAmbientIntensity());
	glUniform3fv(glGetUniformLocation(shader->m_program, "lightSpecColor"), 1, &lightSources[0]->getSpecColor()[0]);
	glUniform3fv(glGetUniformLocation(shader->m_program, "lightPosition"), 1, &lightSources[0]->getPosition()[0]);
	glUniform3fv(glGetUniformLocation(shader->m_program, "lightColor"), 1, &lightSources[0]->getColour()[0]);
	glUniform1f(glGetUniformLocation(shader->m_program, "specPower"), lightSources[0]->getSpecIntensity());
	glUniform3fv(glGetUniformLocation(shader->m_program, "camPos"), 1, &m_cameraPosition[0]);
	// When we setup the geometry, we did a bunch of glEnableVertexAttribArray and glVertexAttribPointer method calls
	// we also Bound the vertex array and index array via the glBindBuffer call.
	// if we where not using VAO's we would have to do thoes method calls each frame here.
	glBindVertexArray(m_Vao);
	// Step 4: Draw Elements. We are using GL_TRIANGLES.
	// we need to tell openGL how many indices there are, and the size of our indices
	// when we setup the geometry, our indices where an unsigned char (1 byte for each indicy)
	//glDrawElements(GL_TRIANGLES, m_cubeIndicesCount, GL_UNSIGNED_BYTE, 0);
	//---No longer drawing the indices...now drawing landscape---
	DrawLandscape();
	// Step 5: Now that we are done drawing the geometry
	// unbind the vao, we are basically cleaning the opengl state
	glBindVertexArray(0);
	// Step 6: de-activate the shader program, dont do future rendering with it any more.
	glUseProgram(0);
	#pragma endregion

	for (auto& member : gameModels)
	{
		member->Draw(projectionView,lightSources,m_camera);
	}
	

	#pragma region Particles
	particleShader->Bind();
	int loc = glGetUniformLocation(particleShader->m_program,
		"projectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE,
		glm::value_ptr(projectionView));

	m_emitter->Draw();
	//FBX - END
	glUseProgram(0);
	#pragma endregion

	postProcessor->DrawPostProcess(postProcessor->m_enablePostProcess, getWindowHeight(), getWindowWidth());
}

void LandscapeApp::Vertex::SetupVertexAttribPointers()
{
	// enable vertex position element
	// notice when we loaded the shader, we described the "position" element to be location 0.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,                  // attribute 0 (position)
		4,                  // size - how many floats make up the position (x, y, z, w)
		GL_FLOAT,           // type - our x,y,z, w are float values
		GL_FALSE,           // normalized? - not used
		sizeof(Vertex),     // stride - size of an entire vertex
		(void*)0            // offset - bytes from the beginning of the vertex
	);
	//tex coords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,                  
		2,                 
		GL_FLOAT,           
		GL_FALSE,           
		sizeof(Vertex),     
		(void*)(sizeof(float) * 4)
	);
	//vertex normal
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2,
		4,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(sizeof(float) * 6)
	);
}

void LandscapeApp::CreateLandscape()
{
	std::vector<Vertex> verts;
	std::vector<unsigned int> indices;

	const unsigned char* pixels = m_heightMap->getPixels();
	
	//Create grid of vertices
	for (int i = 0; i < M_LAND_DEPTH; i++)
	{
		for (int j = 0; j < M_LAND_WIDTH; j++)
		{

			int sampleX = (float) j / M_LAND_WIDTH * m_heightMap->getWidth();
			int sampleZ = (float) i / M_LAND_DEPTH * m_heightMap->getHeight();


			int k = sampleZ * m_heightMap->getWidth() + sampleX;

			//position of vertex
			float xPos = (j * m_vertSeperation) - (M_LAND_WIDTH * m_vertSeperation * 0.5f);
			float yPos = (pixels[k * 3] / 255.0f) * m_maxHeight;
			float zPos = (i * m_vertSeperation) - (M_LAND_DEPTH * m_vertSeperation * 0.5f);

			float u = (float)j / (M_LAND_WIDTH - 1);
			float v = (float)i / (M_LAND_DEPTH - 1);
			Vertex vert{
				glm::vec4(xPos,yPos,zPos,1.0f),//Position
				glm::vec2(u,v),//uv coords
				glm::vec4(0.0f,1.0f,0.0f,0.0f)//normals
			};
			verts.push_back(vert);
		}
	}
	glm::vec3 normals;
	//calculate indices for triangles
	for (int i = 0;i < M_LAND_DEPTH - 1;i++)
	{
		for (int j = 0; j < M_LAND_WIDTH - 1; j++)
		{
			int k = i * M_LAND_WIDTH + j;//the address of the vertices in the single dimesion vector
			
			indices.push_back(k + 1);						//b--a
			indices.push_back(k);							//| /
			indices.push_back(k + M_LAND_WIDTH);			//
			
			indices.push_back(k + 1);			//				 a
			indices.push_back(k + M_LAND_WIDTH);//				/ |
			indices.push_back(k + M_LAND_WIDTH + 1);		// c--d

			glm::vec3 A = (glm::vec3)(verts[k + 1].pos);
			glm::vec3 B = (glm::vec3)(verts[k].pos);
			glm::vec3 C = (glm::vec3)(verts[k + M_LAND_WIDTH].pos);
			glm::vec3 D = (glm::vec3)(verts[k + M_LAND_WIDTH + 1].pos);
			normals = glm::cross(C - B, A - B);
			normals += glm::cross(B - A, C - A);
			normals += glm::cross(B - C, A - C);

			normals += glm::cross(D - A, C - A);
			normals += glm::cross(D - C, A - C);
			normals += glm::cross(A - D, C - D);

			glm::normalize(normals);
			verts[k].vNormal = glm::vec4(normals, 0.0f);

		}
	}

	m_vertCount = verts.size();
	m_IndicesCount = indices.size();

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

	glBufferData(GL_ARRAY_BUFFER, m_vertCount * sizeof(Vertex), &verts[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_IndicesCount * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	Vertex::SetupVertexAttribPointers();

	//unbind after we have finished using them
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


}

void LandscapeApp::DestroyLandscape()
{
	delete m_texture;
	delete m_heightMap;
}

void LandscapeApp::DrawLandscape()
{
	glDrawElements(GL_TRIANGLES, m_IndicesCount, GL_UNSIGNED_INT, 0);
}

//void LandscapeApp::CreateFBXOpenGLBuffers(FBXFile * fbx)
//{
//	// FBX Files contain multiple meshes, each with seperate material information
//	// loop through each mesh within the FBX file and cretae VAO, VBO and IBO buffers for each mesh.
//	// We can store that information within the mesh object via its "user data" void pointer variable.
//		for (unsigned int i = 0; i < fbx->getMeshCount(); i++)
//		{
//			// get the current mesh from file
//			FBXMeshNode *fbxMesh = fbx->getMeshByIndex(i);
//			GLMesh *glData = new GLMesh();
//			glGenVertexArrays(1, &glData->vao);
//			glBindVertexArray(glData->vao);
//			glGenBuffers(1, &glData->vbo);
//			glGenBuffers(1, &glData->ibo);
//			glBindBuffer(GL_ARRAY_BUFFER, glData->vbo);
//			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData->ibo);
//			// fill the vbo with our vertices.
//			// the FBXLoader has convinently already defined a Vertex Structure for us.
//			glBufferData(GL_ARRAY_BUFFER,
//				fbxMesh->m_vertices.size() * sizeof(FBXVertex),
//				fbxMesh->m_vertices.data(), GL_STATIC_DRAW);
//			// fill the ibo with the indices.
//			// fbx meshes can be large, so indices are stored as an unsigned int.
//			glBufferData(GL_ELEMENT_ARRAY_BUFFER,
//				fbxMesh->m_indices.size() * sizeof(unsigned int),
//				fbxMesh->m_indices.data(), GL_STATIC_DRAW);
//			// Setup Vertex Attrib pointers
//			// remember, we only need to setup the approprate attributes for the shaders that will be rendering
//			// this fbx object.
//			glEnableVertexAttribArray(0); // position
//			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), 0);
//			glEnableVertexAttribArray(1); // normal
//			glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::NormalOffset);
//			glEnableVertexAttribArray(2); // uv
//			glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::TexCoord1Offset);
//			// TODO: add any additional attribute pointers required for shader use.
//			// unbind
//			glBindVertexArray(0);
//			glBindBuffer(GL_ARRAY_BUFFER, 0);
//			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//			// attach our GLMesh object to the m_userData pointer.
//			fbxMesh->m_userData = glData;
//		}
//}
//
//void LandscapeApp::CreateFBXAnimatedOpenGLBuffers(FBXFile * fbx)
//{
//	// FBX Files contain multiple meshes, each with seperate material information
//	// loop through each mesh within the FBX file and cretae VAO, VBO and IBO buffers for each mesh.
//	// We can store that information within the mesh object via its "user data" void pointer variable.
//	for (unsigned int i = 0; i < fbx->getMeshCount(); i++)
//	{
//		// get the current mesh from file
//		FBXMeshNode *fbxMesh = fbx->getMeshByIndex(i);
//		GLMesh *glData = new GLMesh();
//		glGenVertexArrays(1, &glData->vao);
//		glBindVertexArray(glData->vao);
//		glGenBuffers(1, &glData->vbo);
//		glGenBuffers(1, &glData->ibo);
//		glBindBuffer(GL_ARRAY_BUFFER, glData->vbo);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData->ibo);
//		// fill the vbo with our vertices.
//		// the FBXLoader has convinently already defined a Vertex Structure for us.
//		glBufferData(GL_ARRAY_BUFFER,
//			fbxMesh->m_vertices.size() * sizeof(FBXVertex),
//			fbxMesh->m_vertices.data(), GL_STATIC_DRAW);
//		// fill the ibo with the indices.
//		// fbx meshes can be large, so indices are stored as an unsigned int.
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
//			fbxMesh->m_indices.size() * sizeof(unsigned int),
//			fbxMesh->m_indices.data(), GL_STATIC_DRAW);
//		// Setup Vertex Attrib pointers
//		// remember, we only need to setup the approprate attributes for the shaders that will be rendering
//		// this fbx object.
//		glEnableVertexAttribArray(0); //position
//		glEnableVertexAttribArray(1); //normals
//		glEnableVertexAttribArray(2); //tangents
//		glEnableVertexAttribArray(3); //texcoords
//		glEnableVertexAttribArray(4); //weights
//		glEnableVertexAttribArray(5); //indices
//		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex),
//			(void*)FBXVertex::PositionOffset);
//		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex),
//			(void*)FBXVertex::NormalOffset);
//		glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex),
//			(void*)FBXVertex::TangentOffset);
//		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex),
//			(void*)FBXVertex::TexCoord1Offset);
//		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex),
//			(void*)FBXVertex::WeightsOffset);
//		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex),
//			(void*)FBXVertex::IndicesOffset);
//		// TODO: add any additional attribute pointers required for shader use.
//		// unbind
//		glBindVertexArray(0);
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//		// attach our GLMesh object to the m_userData pointer.
//		fbxMesh->m_userData = glData;
//	}
//}
//
//void LandscapeApp::CleanupFBXOpenGLBuffers(FBXFile * file)
//{
//	for (unsigned int i = 0; i < file->getMeshCount(); i++)
//	{
//		FBXMeshNode *fbxMesh = file->getMeshByIndex(i);
//		GLMesh *glData = (GLMesh *)fbxMesh->m_userData;
//		glDeleteVertexArrays(1, &glData->vao);
//		glDeleteBuffers(1, &glData->vbo);
//		glDeleteBuffers(1, &glData->ibo);
//		delete glData;
//	}
//}
//
//void LandscapeApp::PlayAnimationTo(int a, int b)
//{
//	animation->m_startFrame = a;
//	animation->m_endFrame = b;
//	fbxCurrentFrame = animation->m_startFrame;
//}

/*void LandscapeApp::LoadShader()
{
static const char* vertex_shader =
"#version 400\n								\
in vec4 vPosition;\n							\
in vec2 vUv;\n									\
in vec4 vNormal;\n								\
out vec2 fUv;\n 								\
out vec3 fPos;\n 								\
out vec4 fNormal;\n 							\
uniform mat4 projectionView; \n					\
void main ()\n									\
{\n												\
fNormal = vNormal;\n						\
fPos = vPosition.xyz;\n						\
fUv = vUv;\n								\
gl_Position = projectionView * vPosition;\n	\
}";

static const char* fragment_shader =
"#version 400\n											\
in vec2 fUv;\n												\
in vec3 fPos;\n 											\
in vec4 fNormal;\n 											\
out vec4 frag_color;\n										\
uniform sampler2D texture;\n								\
uniform float lightAmbientStrength;\n						\
uniform vec3 lightPosition;\n								\
uniform vec3 lightColor;\n									\
void main ()\n												\
{\n															\
vec3 norm = normalize(fNormal.xyz);\n						\
vec3 lightDir = normalize(fPos - lightPosition);\n		\
float diff = max(dot(norm,lightDir),0.0f);\n				\
vec3 diffColor = diff * lightColor;\n						\
vec3 ambient = lightColor * lightAmbientStrength;\n		\
frag_color = texture2D(texture,fUv) * vec4(ambient + diffColor,1.0);\n\
}";

// Step 1:
// Load the vertex shader, provide it with the source code and compile it.
GLuint vs = glCreateShader(GL_VERTEX_SHADER);
glShaderSource(vs, 1, &vertex_shader, NULL);
glCompileShader(vs);

// Step 2:
// Load the fragment shader, provide it with the source code and compile it.
GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
glShaderSource(fs, 1, &fragment_shader, NULL);
glCompileShader(fs);

// step 3:
// Create the shader program
m_shader = glCreateProgram();

// Step 4:
// attach the vertex and fragment shaders to the m_shader program
glAttachShader(m_shader, vs);
glAttachShader(m_shader, fs);

// Step 5:
// describe the location of the shader inputs the link the program
glBindAttribLocation(m_shader, 0, "vPosition");
glBindAttribLocation(m_shader, 1, "vUv");
glBindAttribLocation(m_shader, 2, "vNormal");

glLinkProgram(m_shader);

// step 6:
// delete the vs and fs shaders
glDeleteShader(vs);
glDeleteShader(fs);


}*/

/*void LandscapeApp::UnloadShader()
{
glDeleteProgram(shader->m_program);
}*/

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
