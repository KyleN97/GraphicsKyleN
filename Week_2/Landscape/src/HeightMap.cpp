#include "HeightMap.h"
#include <gl_core_4_4.h>
#include <glm/ext.hpp>
#include <imgui.h>
HeightMap::HeightMap()
{
	m_shader = new Shader("Landscape/Shaders/basicShader");
	for (size_t i = 0; i < NUM_ITEMS; i++)
	{
		m_textures.push_back(NULL);
	}

	//---load heightmap---
	m_textures[heightmap] = new aie::Texture();
	m_textures[heightmap]->load("Landscape/Textures/heightmap.bmp");
	//---load grass---
	m_textures[grass] = new aie::Texture();
	m_textures[grass]->load("Landscape/Textures/grass.png");
	//---load rock---
	m_textures[rock] = new aie::Texture();
	m_textures[rock]->load("Landscape/Textures/rock.png");
	//---load sand---
	m_textures[sand] = new aie::Texture();
	m_textures[sand]->load("Landscape/Textures/sand.png");
	//---load snow---
	m_textures[snow] = new aie::Texture();
	m_textures[snow]->load("Landscape/Textures/snow.png");
	//---load splat--
	m_textures[splat] = new aie::Texture();
	m_textures[splat]->load("Landscape/Textures/splat.jpg");

	m_textures[water] = new aie::Texture();
	m_textures[water]->load("Landscape/Textures/water.png");
	CreateHeightMap();
}


HeightMap::~HeightMap()
{
	DestroyHeightMap();
}

void HeightMap::CreateHeightMap()
{
	std::vector<Vertex> verts;
	std::vector<unsigned int> indices;

	const unsigned char* pixels = m_textures[heightmap]->getPixels();

	//Create grid of vertices
	for (int i = 0; i < m_LandDepth; i++)
	{
		for (int j = 0; j < m_LandWidth; j++)
		{

			int sampleX = (float)j / m_LandWidth * m_textures[heightmap]->getWidth();
			int sampleZ = (float)i / m_LandDepth * m_textures[heightmap]->getHeight();


			int k = sampleZ * m_textures[heightmap]->getWidth() + sampleX;

			//position of vertex
			float xPos = (j * m_vertSeperation) - (m_LandWidth * m_vertSeperation * 0.5f);
			float yPos = (pixels[k * 3] / 255.0f) * m_maxHeight;
			float zPos = (i * m_vertSeperation) - (m_LandDepth * m_vertSeperation * 0.5f);

			float u = (float)j / (m_LandWidth - 1);
			float v = (float)i / (m_LandDepth - 1);
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
	for (int i = 0; i < m_LandDepth - 1; i++)
	{
		for (int j = 0; j < m_LandWidth - 1; j++)
		{
			int k = i * m_LandWidth + j;//the address of the vertices in the single dimesion vector

			indices.push_back(k + 1);						//b--a
			indices.push_back(k);							//| /
			indices.push_back(k + m_LandWidth);			//

			indices.push_back(k + 1);			//				 a
			indices.push_back(k + m_LandWidth);//				/ |
			indices.push_back(k + m_LandWidth + 1);		// c--d

			glm::vec3 A = (glm::vec3)(verts[k + 1].pos);
			glm::vec3 B = (glm::vec3)(verts[k].pos);
			glm::vec3 C = (glm::vec3)(verts[k + m_LandWidth].pos);
			glm::vec3 D = (glm::vec3)(verts[k + m_LandWidth + 1].pos);
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

void HeightMap::DrawHeightMap(glm::mat4 projectionView, std::vector<Light*> lightSources,Camera* camera)
{
	ImGui::Begin("Water Editor");
	ImGui::SliderFloat("Water Frequency",&waterFrequency ,0, 10);
	ImGui::SliderFloat("Water Amplitude",&waterAmplitude ,0, 10);
	ImGui::SliderFloat("Water Speed",&waterSpeed ,0, 10);

	ImGui::End();
#pragma region BindTextures
	//setup texture in open gl - select the first texture as active, then bind it 
	//also set it up as a uniform variable for shader
	m_shader->Bind();
	//bind the shader and use it
	//setup grass texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_textures[grass]->getHandle());
	glUniform1i(glGetUniformLocation(m_shader->m_program, "grass"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_textures[sand]->getHandle());
	glUniform1i(glGetUniformLocation(m_shader->m_program, "sand"), 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_textures[snow]->getHandle());
	glUniform1i(glGetUniformLocation(m_shader->m_program, "snow"), 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_textures[rock]->getHandle());
	glUniform1i(glGetUniformLocation(m_shader->m_program, "rock"), 3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_textures[splat]->getHandle());
	glUniform1i(glGetUniformLocation(m_shader->m_program, "splat"), 4);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, m_textures[water]->getHandle());
	glUniform1i(glGetUniformLocation(m_shader->m_program, "water"), 5);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glUniformMatrix4fv(
		glGetUniformLocation(m_shader->m_program, "projectionView"),
		1,
		false,
		glm::value_ptr(projectionView));
	// Step 3: Bind the VAO
	glUniform1f(glGetUniformLocation(m_shader->m_program, "lightAmbientStrength"), lightSources[0]->getAmbientIntensity());
	glUniform3fv(glGetUniformLocation(m_shader->m_program, "lightSpecColor"), 1, &lightSources[0]->getSpecColor()[0]);
	glUniform3fv(glGetUniformLocation(m_shader->m_program, "lightPosition"), 1, &lightSources[0]->getPosition()[0]);
	glUniform3fv(glGetUniformLocation(m_shader->m_program, "lightColor"), 1, &lightSources[0]->getColour()[0]);
	glUniform1f(glGetUniformLocation(m_shader->m_program, "specPower"), lightSources[0]->getSpecIntensity());
	glUniform3fv(glGetUniformLocation(m_shader->m_program, "camPos"), 1, &camera->GetPos()[0]);
	glUniform1f(glGetUniformLocation(m_shader->m_program, "Time"), timePassed);
	glUniform1f(glGetUniformLocation(m_shader->m_program, "amplitude"), waterAmplitude);
	glUniform1f(glGetUniformLocation(m_shader->m_program, "frequency"), waterFrequency);
	glUniform1f(glGetUniformLocation(m_shader->m_program, "speed"), waterSpeed);
	//glUniform1f(glGetUniformLocation(m_shader->m_program, "blend"), blend);
//
	// When we setup the geometry, we did a bunch of glEnableVertexAttribArray and glVertexAttribPointer method calls
	// we also Bound the vertex array and index array via the glBindBuffer call.
	// if we where not using VAO's we would have to do thoes method calls each frame here.
	glBindVertexArray(m_Vao);
	// Step 4: Draw Elements. We are using GL_TRIANGLES.
	// we need to tell openGL how many indices there are, and the size of our indices
	// when we setup the geometry, our indices where an unsigned char (1 byte for each indicy)
	//glDrawElements(GL_TRIANGLES, m_cubeIndicesCount, GL_UNSIGNED_BYTE, 0);
	//---No longer drawing the indices...now drawing landscape---
	glDrawElements(GL_TRIANGLES, m_IndicesCount, GL_UNSIGNED_INT, 0);
	// Step 5: Now that we are done drawing the geometry
	// unbind the vao, we are basically cleaning the opengl state
	glBindVertexArray(0);
	// Step 6: de-activate the shader program, dont do future rendering with it any more.
	glUseProgram(0);


#pragma endregion
}

void HeightMap::DestroyHeightMap()
{
	for (int i = 0; i < NUM_ITEMS; i++)
	{
		delete m_textures[i];
	}
}

void HeightMap::Vertex::SetupVertexAttribPointers()
{
	 //enable vertex position element
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
