

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
#include <imgui.h>
#include "gl_core_4_4.h"
#include "Shader.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

LandscapeApp::LandscapeApp() {

}

LandscapeApp::~LandscapeApp() {

}

bool LandscapeApp::startup() {
	

	setBackgroundColour(0.25f, 0.25f, 0.25f);
	const glm::mat4* transform;
	//---initialize gizmo primitive counts---
	Gizmos::create(10000, 10000, 10000, 10000);

	//---Setup camera starting position and where it's looking---
	m_camera = new Camera();
	m_camera->SetPosition(glm::vec3(5.0f,5.0f,5.0f));
	m_camera->LookAt(glm::vec3(0.0f,0.0f,0.0f));

	//---setup light---
	m_lightPosition = glm::vec3(0.0f,5.0f,0.0f);
	m_lightColor = glm::vec3(1,1,1);
	m_lightAmbientStrength = 0.05f;

	//---load texture---
	m_texture = new aie::Texture();
	m_texture->load("Landscape/Textures/Tile.png");



	//---load heightmap---
	m_heightMap = new aie::Texture();
	m_heightMap->load("Landscape/Textures/heightmap.bmp");
	m_grass = new aie::Texture();
	m_grass->load("Landscape/Textures/grass.png");
	m_rock = new aie::Texture();
	m_rock->load("Landscape/Textures/rock.png");
	m_sand = new aie::Texture();
	m_sand->load("Landscape/Textures/sand.png");
	m_snow = new aie::Texture();
	m_snow->load("Landscape/Textures/snow.png");
	m_splat = new aie::Texture();
	m_splat->load("Landscape/Textures/splat.jpg");
	//LoadShader();

	//--Load in shader from file and check the errors and put to console---
	shader = new Shader("Landscape/Shaders/basicShader");
	objectPosition.reserve(64);
	objectScale.reserve(64);
	createObject.reserve(64);

	objectPosition.push_back(glm::vec3(0, 0, 0));
	objectScale.push_back(0);
	createObject.push_back(false);
	//---Create the landscape---
	CreateLandscape();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	return true;
}

void LandscapeApp::shutdown() {
	DestroyLandscape();
	delete shader;
	delete m_camera;
	//Gizmos::destroy();
	//UnloadShader();

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
	ImGui::Begin("Lighting Editor"); // begin second window
	ImGui::SliderFloat("Specular Strength", &m_specPower, 0, 1000);
	ImGui::Text("Light Position");
	ImGui::Text(glm::to_string(m_lightPosition).c_str());
	ImGui::Text("Camera Position");
	ImGui::Text(glm::to_string(m_camera->GetPos()).c_str());
	ImGui::ColorEdit3("Light Color", glm::value_ptr(m_lightColor));
	ImGui::ColorEdit3("Spec Light Color", glm::value_ptr(m_lightSpecColor));


	const mat4 sphereMat = /*glm::rotate(0.0f * time,glm::vec3(0,5,0)) **/ glm::translate(glm::vec3(vec3(glm::sin(time) * 3, 3, glm::cos(time) * 3)));//translate the sphere in an orbit 3 wide and 3 high

	ImGui::End();

	ImGui::Begin("Landscape Editor");
	ImGui::Checkbox("WireFrame", &m_isWireframe);
	ImGui::End();
	ImGui::Begin("Object Creator");
	ImGui::InputFloat3("Cube Position", glm::value_ptr(objectPosition[amountOfObjects - 1]));
	ImGui::InputFloat("Cube Scale", &objectScale[amountOfObjects - 1]);
	if (ImGui::Button("Create Sphere"))
	{	

			createObject.push_back(false);
			objectPosition.push_back(vec3(0,0,0));
			objectScale.push_back(0.0f);
			createObject[amountOfObjects - 1] = true;
			amountOfObjects++;

	}
	ImGui::End();

	ImGui::Begin("Debugger");
	int count = 0;
	std::string frameRatestr = "Average Framerate " + std::to_string(1.0f / deltaTime);
	ImGui::Text(frameRatestr.c_str());
	ImGui::End();
	m_camera->Update(deltaTime);
	int counter = 0;
	for each (bool var in createObject)
	{
		counter++;
		if (var == true)
		{
			CreateObject(objectPosition[counter - 1], objectScale[counter - 1]);

		}
	}

	Gizmos::addSphere(vec3(0, 0, 0), .5, 64, 12, vec4(1, 0, 0, 0.5f), &sphereMat);
	m_lightPosition = /*glm::vec3(vec3(glm::sin(time) * 10, 10, glm::cos(time) * 10))*/sphereMat[3].xyz;
	m_cameraPosition = m_camera->GetPos();
	shader->Bind();
	// wipe the gizmos clean for this frame

	//Gizmos::addTransform(mat4(1));



	//DrawGrid();
	
	shader->Bind();
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
	
	// wipe the screen to the background colour
	clearScreen();
	glPolygonMode(GL_FRONT_AND_BACK,GL_FRONT);
	
	// update perspective in case window resized
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
										  getWindowWidth() / (float)getWindowHeight(),
										  0.1f, 1000.f);

	// STEP 1: enable the shader program for rendering
	glUseProgram(shader->m_program);

	// Step 2: send uniform variables to the shader
	glm::mat4 projectionView = m_projectionMatrix * m_camera->GetView();
	glUniformMatrix4fv(
		glGetUniformLocation(shader->m_program, "projectionView"),
		1,
		false,
		glm::value_ptr(projectionView));
	
	//setup texture in open gl - select the first texture as active, then bind it 
	//also set it up as a uniform variable for shader
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture->getHandle());
	glUniform1i(glGetUniformLocation(shader->m_program,"texture"),0);

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

	// Step 3: Bind the VAO
	glUniform1f(glGetUniformLocation(shader->m_program, "lightAmbientStrength"), m_lightAmbientStrength);
	glUniform3fv(glGetUniformLocation(shader->m_program, "lightPosition"),1, &m_lightPosition[0]);
	glUniform3fv(glGetUniformLocation(shader->m_program, "lightSpecColor"), 1, &m_lightSpecColor[0]);
	glUniform3fv(glGetUniformLocation(shader->m_program, "lightColor"), 1, &m_lightColor[0]);
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

	Gizmos::draw(m_projectionMatrix * m_camera->GetView());
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

void LandscapeApp::CreateObject(glm::vec3 position, float scale)
{
	Gizmos::addSphere(position, scale, 32, 32, vec4(1, 0, 0, 0.5f), NULL);

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
