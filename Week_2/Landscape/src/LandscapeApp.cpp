

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
using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

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
	m_lightAmbientStrength = 0.05f;
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
	m_myFbxModel = new FBXFile();
	m_myFbxModel->load("./models/soulspear/soulspear.fbx", FBXFile::UNITS_CENTIMETER);
	CreateFBXOpenGLBuffers(m_myFbxModel);
#pragma endregion
	#pragma region ShaderSetup
	fbxShader = new Shader("Landscape/Shaders/fbxShader");
	shader = new Shader("Landscape/Shaders/basicShader");
	particleShader = new Shader("Landscape/Shaders/particleShader");
	frameBufferShader = new Shader("Landscape/Shaders/frameBufferShader");
#pragma endregion
	#pragma region ParticlesSetup
	m_emitter = new ParticleEmitter();
	m_emitter->Init(1000, 500, 0.1f, 1.0f, 1, 5, 1, 0.1f, glm::vec4(1, 0, 0, 1), glm::vec4(1, 1, 0, 1), glm::vec3(2, 2, 2));
#pragma endregion
	#pragma region ObjectCreatorSetup
	objectPosition.reserve(64);
	objectScale.reserve(64);
	createObject.reserve(64);
	objectColor.reserve(64);
	objectPosition.push_back(glm::vec3(1, 1, 1));
	objectScale.push_back(1);
	objectColor.push_back(glm::vec4(1, 1, 1, 1));
	createObject.push_back(false);
#pragma endregion
	setBackgroundColour(0.25f, 0.25f, 0.25f);
	//---initialize gizmo primitive counts---
	Gizmos::create(10000, 10000, 10000, 10000);
	m_positions[0] = glm::vec3(10, 5, 10);
	m_positions[1] = glm::vec3(-10, 0, -10);
	m_rotations[0] = glm::quat(glm::vec3(0,-1, 0));
	m_rotations[1] = glm::quat(glm::vec3(0, 1, 0));

	SetupFrameBuffer();
	SetupFrameQuad();

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
	CleanupFBXOpenGLBuffers(m_myFbxModel);
	m_myFbxModel->unload();
	delete m_myFbxModel;
}

void LandscapeApp::update(float deltaTime) {


	if (m_isWireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	}
	Gizmos::clear();
	// query time since application started
	float time = getTime();
	static float wrap_width = 200.0f;

	#pragma region Lighting GUI
	ImGui::Begin("Lighting Editor");
	ImGui::SliderFloat("Specular Strength", &m_specPower, 0, 10000);
	ImGui::ColorEdit3("Light Color", glm::value_ptr(lightSources[0]->colour));
	ImGui::ColorEdit3("Spec Light Color", glm::value_ptr(lightSources[0]->specColor));
	ImGui::End();
	#pragma endregion
	
	#pragma region Landscape GUI
	ImGui::Begin("Landscape Editor");
	ImGui::Checkbox("WireFrame", &m_isWireframe);
	ImGui::End();
	ImGui::Begin("Object Creator");
	#pragma endregion
	
	#pragma region ObjectGUI
	ImGui::InputFloat3("Object Position", glm::value_ptr(objectPosition[amountOfObjects - 1]));
	ImGui::InputFloat("Object Scale", &objectScale[amountOfObjects - 1]);
	ImGui::ColorEdit4("Object Color", glm::value_ptr(objectColor[amountOfObjects - 1]));
	const char* listbox_items[] = { "Sphere","AABBFilled","Disk" };
	static int listbox_item_current = 0;
	ImGui::ListBox("Object Type", &listbox_item_current, listbox_items, sizeof(listbox_items) / sizeof(listbox_items[0]), 4);
	if (ImGui::Button("Create Object"))
	{
		createObject.push_back(false);
		objectPosition.push_back(vec3(1, 1, 1));
		objectScale.push_back(1.0f);
		objectColor.push_back(glm::vec4(1, 1, 1, 1));
		objectType.push_back(listbox_items[listbox_item_current]);
		createObject[amountOfObjects - 1] = true;
		amountOfObjects++;
	}
	ImGui::End();
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
		ImGui::Begin("Post Processing");
		ImGui::Checkbox("Enable Post Processing", &m_enablePostProcess);
		ImGui::End();
		if (m_enablePostProcess)
		{
			ImGui::Begin("Post Processing Settings");
			ImGui::Checkbox("Distortion", &m_enableDistortion);
			ImGui::Checkbox("Blur", &m_enableBlur);
			ImGui::Checkbox("Grey Scale", &m_enableGrey);
			if (m_enableDistortion == false && m_enableBlur == false && m_enableGrey == false)
			{
				m_enablePostProcess = false;
				m_enableDistortion = true;
			}
			ImGui::End();
		}
#pragma endregion

	m_camera->Update(deltaTime);
	m_emitter->Update(deltaTime, m_camera->GetView());

	if (createObject[0] == true){
		DrawAABBFilled();
		DrawRing();
		DrawSphere();
	}

	const mat4 sphereMat = glm::translate(glm::vec3(vec3(glm::sin(time) * 3, 3, glm::cos(time) * 3)));//translate the sphere in an orbit 3 wide and 3 high
	fbxMat = { fbxScale,0.0f , 0.0f , 0.0f,
			   0.0f ,fbxScale, 0.0f , 0.0f,
			   0.0f ,0.0f , fbxScale, 0.0f,
			   0.0f ,0.0f , 0.0f ,fbxScale };

	float s = glm::cos(time) * 0.5f  + 0.5f;
	glm::vec3 p = (1.0f - s) * m_positions[0] + s * m_positions[1];
	glm::quat r = glm::slerp(m_rotations[0],m_rotations[1],s);

	fbxMat = glm::translate(p) * glm::toMat4(r);


	//fbxMat = glm::translate(glm::vec3(vec3(glm::sin(time) * 3)));
	Gizmos::addSphere(vec3(0, 0, 0), .5, 64, 12, vec4(1, 0, 0, 0.5f), &sphereMat);

	lightSources[0]->SetPosition(sphereMat[3].xyz);
	m_cameraPosition = m_camera->GetPos();

	shader->Bind();
	fbxShader->Bind();
	particleShader->Bind();
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
	
	InitDrawPostProcess(m_enablePostProcess);

	// wipe the screen to the background colour
	clearScreen();
	glPolygonMode(GL_FRONT_AND_BACK,GL_FRONT);
	Gizmos::draw(m_projectionMatrix * m_camera->GetView());

	// update perspective in case window resized
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
										  getWindowWidth() / (float)getWindowHeight(),
										  0.1f, 1000.f);
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
	glUseProgram(shader->m_program);

	// Step 2: send uniform variables to the shader
	glm::mat4 projectionView = m_projectionMatrix * m_camera->GetView();
	glUniformMatrix4fv(
		glGetUniformLocation(shader->m_program, "projectionView"),
		1,
		false,
		glm::value_ptr(projectionView));
	// Step 3: Bind the VAO
	glUniform1f(glGetUniformLocation(shader->m_program, "lightAmbientStrength"), m_lightAmbientStrength);
	glUniform3fv(glGetUniformLocation(shader->m_program, "lightSpecColor"), 1, &lightSources[0]->getSpecColor()[0]);
	glUniform3fv(glGetUniformLocation(shader->m_program, "lightPosition"), 1, &lightSources[0]->getPosition()[0]);
	glUniform3fv(glGetUniformLocation(shader->m_program, "lightColor"), 1, &lightSources[0]->getColour()[0]);
	glUniform1f(glGetUniformLocation(shader->m_program, "specPower"), m_specPower);
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
	
	#pragma region UniformsFBX
	//FBX - START
	glUseProgram(fbxShader->m_program);

	// send uniform variables, in this case the "projectionViewWorldMatrix"
	unsigned int mvpLoc = glGetUniformLocation(fbxShader->m_program, "projectionViewWorldMatrix");
	glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(projectionView * fbxMat));
	// loop through each mesh within the fbx file
	for (unsigned int i = 0; i < m_myFbxModel->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = m_myFbxModel->getMeshByIndex(i);
		GLMesh* glData = (GLMesh*)mesh->m_userData;
		// get the texture from the model
		unsigned int diffuseTexture = m_myFbxModel->getTextureByIndex(mesh->m_material->DiffuseTexture);
		// bid the texture and send it to our shader
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseTexture);
		glUniform1i(glGetUniformLocation(fbxShader->m_program, "diffuseTexture"), 0);
		glUniform1f(glGetUniformLocation(shader->m_program, "lightAmbientStrength"), m_lightAmbientStrength);
		glUniform3fv(glGetUniformLocation(shader->m_program, "lightSpecColor"), 1, &lightSources[0]->getSpecColor()[0]);
		glUniform3fv(glGetUniformLocation(shader->m_program, "lightPosition"), 1, &lightSources[0]->getPosition()[0]);
		glUniform3fv(glGetUniformLocation(shader->m_program, "lightColor"), 1, &lightSources[0]->getColour()[0]);
		glUniform1f(glGetUniformLocation(shader->m_program, "specPower"), m_specPower);
		glUniform3fv(glGetUniformLocation(shader->m_program, "camPos"), 1, &m_cameraPosition[0]);


		// draw the mesh
		glBindVertexArray(glData->vao);
		glDrawElements(GL_TRIANGLES, mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	glUseProgram(0);
	#pragma endregion

	#pragma region Particles
	glUseProgram(particleShader->m_program);
	int loc = glGetUniformLocation(particleShader->m_program,
		"projectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE,
		glm::value_ptr(projectionView));

	m_emitter->Draw();
	//FBX - END
	glUseProgram(0);
	#pragma endregion

	
	DrawPostProcess(m_enablePostProcess);
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

void LandscapeApp::DrawAABBFilled()
{
	for (int i = 0; i < amountOfObjects - 1; i++)
	{
		if (objectType[i] == "AABBFilled")
		{
			Gizmos::addAABBFilled(objectPosition[i], glm::vec3(objectScale[i], objectScale[i], objectScale[i]), objectColor[i]);
		}
	}
}

void LandscapeApp::DrawSphere()
{
	for (int i = 0; i < amountOfObjects - 1; i++)
	{
		if (objectType[i] == "Sphere")
		{
			Gizmos::addSphere(objectPosition[i], objectScale[i], 32, 32, objectColor[i], NULL);

		}
	}
}

void LandscapeApp::DrawRing()
{
	for (int i = 0; i < amountOfObjects - 1; i++)
	{
		if (objectType[i] == "Ring")
		{
			Gizmos::addDisk(objectPosition[i], objectScale[i], 32, objectColor[i]);

		}
	}
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

void LandscapeApp::CreateFBXOpenGLBuffers(FBXFile * fbx)
{
	// FBX Files contain multiple meshes, each with seperate material information
	// loop through each mesh within the FBX file and cretae VAO, VBO and IBO buffers for each mesh.
	// We can store that information within the mesh object via its "user data" void pointer variable.
		for (unsigned int i = 0; i < fbx->getMeshCount(); i++)
		{
			// get the current mesh from file
			FBXMeshNode *fbxMesh = fbx->getMeshByIndex(i);
			GLMesh *glData = new GLMesh();
			glGenVertexArrays(1, &glData->vao);
			glBindVertexArray(glData->vao);
			glGenBuffers(1, &glData->vbo);
			glGenBuffers(1, &glData->ibo);
			glBindBuffer(GL_ARRAY_BUFFER, glData->vbo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData->ibo);
			// fill the vbo with our vertices.
			// the FBXLoader has convinently already defined a Vertex Structure for us.
			glBufferData(GL_ARRAY_BUFFER,
				fbxMesh->m_vertices.size() * sizeof(FBXVertex),
				fbxMesh->m_vertices.data(), GL_STATIC_DRAW);
			// fill the ibo with the indices.
			// fbx meshes can be large, so indices are stored as an unsigned int.
			glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				fbxMesh->m_indices.size() * sizeof(unsigned int),
				fbxMesh->m_indices.data(), GL_STATIC_DRAW);
			// Setup Vertex Attrib pointers
			// remember, we only need to setup the approprate attributes for the shaders that will be rendering
			// this fbx object.
			glEnableVertexAttribArray(0); // position
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), 0);
			glEnableVertexAttribArray(1); // normal
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::NormalOffset);
			glEnableVertexAttribArray(2); // uv
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::TexCoord1Offset);
			// TODO: add any additional attribute pointers required for shader use.
			// unbind
			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			// attach our GLMesh object to the m_userData pointer.
			fbxMesh->m_userData = glData;
		}
}

void LandscapeApp::CleanupFBXOpenGLBuffers(FBXFile * file)
{
	for (unsigned int i = 0; i < file->getMeshCount(); i++)
	{
		FBXMeshNode *fbxMesh = file->getMeshByIndex(i);
		GLMesh *glData = (GLMesh *)fbxMesh->m_userData;
		glDeleteVertexArrays(1, &glData->vao);
		glDeleteBuffers(1, &glData->vbo);
		glDeleteBuffers(1, &glData->ibo);
		delete glData;
	}
}

void LandscapeApp::SetupFrameBuffer()
{
	glGenFramebuffers(1, &m_fbo); 
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glGenTextures(1, &m_fboTexture);
	glBindTexture(GL_TEXTURE_2D, m_fboTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_fboTexture, 0);
	glGenRenderbuffers(1, &m_fboDepth); glBindRenderbuffer(GL_RENDERBUFFER, m_fboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, getWindowWidth(), getWindowHeight());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_fboDepth);
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 }; 
	glDrawBuffers(1, drawBuffers);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void LandscapeApp::SetupFrameQuad()
{
	glm::vec2 texelSize = 1.0f / glm::vec2(getWindowWidth(), getWindowHeight());
	// fullscreen quad
	glm::vec2 halfTexel = 1.0f / glm::vec2(getWindowWidth(), getWindowHeight()) * 0.5f;
	float vertexData[] = { -1, -1, 0, 1, halfTexel.x, halfTexel.y,
		1, 1, 0, 1, 1 - halfTexel.x, 1 - halfTexel.y, -1, 1, 0, 1,
		halfTexel.x, 1 - halfTexel.y, -1, -1, 0, 1, halfTexel.x,
		halfTexel.y, 1, -1, 0, 1, 1 - halfTexel.x, halfTexel.y,
		1, 1, 0, 1, 1 - halfTexel.x, 1 - halfTexel.y, };
	glGenVertexArrays(1, &m_vao); 
	glBindVertexArray(m_vao);
	glGenBuffers(1, &m_vbo); 
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo); 
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 6, vertexData, GL_STATIC_DRAW); 
	glEnableVertexAttribArray(0); 
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0); 
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, ((char*)0) + 16);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void LandscapeApp::InitDrawPostProcess(bool isOn)
{
	if (isOn)
	{
		// bind our target
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glViewport(0, 0, getWindowWidth(), getWindowHeight());
		// clear the target 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}

void LandscapeApp::DrawPostProcess(bool isOn)
{
	if (isOn)
	{
		// bind the back-buffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, getWindowWidth(), getWindowHeight());
		// just clear the back-buffer depth as 
		//each pixel will be filled
		glClear(GL_DEPTH_BUFFER_BIT);
		// draw our full-screen quad
		glUseProgram(frameBufferShader->m_program);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_fboTexture);
		int loc2 = glGetUniformLocation(frameBufferShader->m_program, "target");
		glUniform1i(glGetUniformLocation(frameBufferShader->m_program, "distort"), m_enableDistortion);
		glUniform1i(glGetUniformLocation(frameBufferShader->m_program, "blur"), m_enableBlur);
		glUniform1i(glGetUniformLocation(frameBufferShader->m_program, "greyScale"), m_enableGrey);
		glUniform1i(loc2, 0);
		glBindVertexArray(m_vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glUseProgram(0);
	}
}


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
