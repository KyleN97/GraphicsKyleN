

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

#include "LandscapeApp.h"
#include "Gizmos.h"
#include "Input.h"
#include "Camera.h"
#include <Texture.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "gl_core_4_4.h"

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

	// initialize gizmo primitive counts
	//Gizmos::create(10000, 10000, 10000, 10000);

	//Setup camera starting position and where it's looking
	m_camera = new Camera();
	m_camera->SetPosition(glm::vec3(5.0f,5.0f,5.0f));
	m_camera->LookAt(glm::vec3(0.0f,0.0f,0.0f));
	//load texture
	m_texture = new aie::Texture();
	m_texture->load("Landscape/Textures/Tile.png");

	//load heightmap
	m_heightMap = new aie::Texture();
	m_heightMap->load("Landscape/Textures/heightmap.bmp");
	LoadShader();
	CreateLandscape();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return true;
}

void LandscapeApp::shutdown() {
	DestroyLandscape();
	UnloadShader();

	//Gizmos::destroy();
	delete m_camera;
}

void LandscapeApp::update(float deltaTime) {

	// query time since application started
	float time = getTime();

	//Control Camera
	m_camera->Update(deltaTime);

	// wipe the gizmos clean for this frame
	//Gizmos::clear();

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
	
	// wipe the screen to the background colour
	clearScreen();
	glPolygonMode(GL_FRONT_AND_BACK,GL_FRONT);
	
	// update perspective in case window resized
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
										  getWindowWidth() / (float)getWindowHeight(),
										  0.1f, 1000.f);

	// STEP 1: enable the shader program for rendering
	glUseProgram(m_shader);

	// Step 2: send uniform variables to the shader
	glm::mat4 projectionView = m_projectionMatrix * m_camera->GetView();
	glUniformMatrix4fv(
		glGetUniformLocation(m_shader, "projectionView"), 
		1, 
		false, 
		glm::value_ptr(projectionView));
	//setup texture in open gl - select the first texture as active, then bind it 
	//also set it up as a uniform variable for shader
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture->getHandle());
	glUniform1i(glGetUniformLocation(m_shader,"texture"),0);
	// Step 3: Bind the VAO
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

	//Gizmos::draw(m_projectionMatrix * m_camera->GetView());
}

void LandscapeApp::LoadShader()
{
	static const char* vertex_shader =
		"#version 400\n								\
	in vec4 vPosition;\n							\
	in vec2 vUv;\n									\
	out vec2 fUv;\n 								\
	uniform mat4 projectionView; \n					\
	void main ()\n									\
	{\n												\
		fUv = vUv;\n								\
	  gl_Position = projectionView * vPosition;\n	\
	}";												

	static const char* fragment_shader =
		"#version 400\n						\
	in vec2 fUv;\n							\
	out vec4 frag_color;\n					\
	uniform sampler2D texture;\n			\
	void main ()\n							\
	{\n										\
	  frag_color = texture2D(texture,fUv);\n\
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
	glLinkProgram(m_shader);

	// step 6:
	// delete the vs and fs shaders
	glDeleteShader(vs);
	glDeleteShader(fs);


}

void LandscapeApp::UnloadShader()
{
	glDeleteProgram(m_shader);
}

void LandscapeApp::CreateCube()
{
	//Vertex verts[] = {
	//
	//	// POSITION						COLOR
	//	// FRONT FACE				  - RED
	//	{glm::vec4(-0.5f,-0.5f, 0.5f, 1.0f),glm::vec4(1.0f, 0.0f, 0.0f, 0.5f)},	// 0
	//	{glm::vec4(0.5f,-0.5f, 0.5f, 1.0f),glm::vec4(1.0f, 0.0f, 0.0f, 0.5f)},	// 1
	//	{glm::vec4(0.5f, 0.5f, 0.5f, 1.0f),glm::vec4(1.0f, 0.0f, 0.0f, 0.5f)},	// 2
	//	{glm::vec4(-0.5f, 0.5f, 0.5f, 1.0f),glm::vec4(1.0f, 0.0f, 0.0f, 0.5f)},	// 3
	//};

	//unsigned char indices[] = {
	//	0, 1, 2,	0, 2, 3			// front facme
	//};
	//
	//m_IndicesCount = sizeof(indices) / sizeof(unsigned char);
	//
	//// Generate the VAO and Bind bind it.
	//// Our VBO (vertex buffer object) and IBO (Index Buffer Object) will be "grouped" with this VAO
	//// other settings will also be grouped with the VAO. this is used so we can reduce draw calls in the render method.
	//glGenVertexArrays(1, &m_Vao);
	//glBindVertexArray(m_Vao);
	//
	//// Create our VBO and IBO.
	//// Then tell Opengl what type of buffer they are used for
	//// VBO a buffer in graphics memory to contains our vertices
	//// IBO a buffer in graphics memory to contain our indices.
	//// Then Fill the buffers with our generated data.
	//// This is taking our verts and indices from ram, and sending them to the graphics card
	//glGenBuffers(1, &m_Vbo);
	//glGenBuffers(1, &m_Ibo);
	//
	//glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ibo);
	//
	//glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	//
	//Vertex::SetupVertexAttribPointers();
	//
	//glBindVertexArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,                  
		2,                 
		GL_FLOAT,           
		GL_FALSE,           
		sizeof(Vertex),     
		(void*)(sizeof(float) * 4)
	);
}

void LandscapeApp::DestroyCube()
{
	// When We're Done, destroy the geometry
	glDeleteBuffers(1, &m_Ibo);
	glDeleteBuffers(1, &m_Vbo);
	glDeleteVertexArrays(1, &m_Vao);
}

void LandscapeApp::CreateLandscape()
{
	std::vector<Vertex> verts;
	std::vector<unsigned short> indices;

	const unsigned char* pixels = m_heightMap->getPixels();
	
	//Create grid of vertices
	for (int i = 0; i < M_LAND_DEPTH; i++)
	{
		for (int j = 0; j < M_LAND_WIDTH; j++)
		{
			int k = i * M_LAND_WIDTH + j;

			//position of vertex
			float xPos = (j * 0.1f) - (M_LAND_WIDTH * 0.1f * 0.5f);
			float yPos = (pixels[k * 3] / 255.0f) * 3;
			float zPos = (i * 0.1f) - (M_LAND_DEPTH * 0.1f * 0.5f);

			float u = (float)j / (M_LAND_WIDTH - 1);
			float v = (float)i / (M_LAND_DEPTH - 1);
			Vertex vert{
				glm::vec4(xPos,yPos,zPos,1.0f),//Position
				//glm::vec4(1,1,1,1)//Colour
				glm::vec2(u,v)
			};
			verts.push_back(vert);
		}
	}
	//calculate indices for triangles
	for (int i = 0;i < M_LAND_DEPTH - 1;i++)
	{
		for (int j = 0; j < M_LAND_WIDTH - 1; j++)
		{
			int k = i * M_LAND_WIDTH + j;//the address of the vertices in the single dimesion vector
			
			indices.push_back(k + 1);			//b--a
			indices.push_back(k);				//| /
			indices.push_back(k + M_LAND_WIDTH);//c

			indices.push_back(k + 1);			//	  a
			indices.push_back(k + M_LAND_WIDTH);//  / |
			indices.push_back(k +M_LAND_WIDTH+ 1);			// b--c
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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_IndicesCount * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

	Vertex::SetupVertexAttribPointers();

	//unbind after we have finished using them
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


}

void LandscapeApp::DestroyLandscape()
{
}

void LandscapeApp::DrawLandscape()
{
	glDrawElements(GL_TRIANGLES, m_IndicesCount, GL_UNSIGNED_SHORT, 0);
	//CreateLandscape();

}
