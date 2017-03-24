#include "GameObject.h"
#include "imgui.h"
#include "Gizmos.h"
#include <glm/ext.hpp>
#include <gl_core_4_4.h>
GameObject::GameObject()
{
	gameObjects.push_back(new Object());
	//Create an empty game object ready to use 
}

GameObject::~GameObject()
{
	for (auto&member : gameObjects)
	{
		delete member;
	}
	for (auto&member : objectTextures)
	{
		delete member;
	}
	for (auto&member : objectData)
	{
		delete member;
	}
}

void GameObject::Create()
{
	if (gameObjects[amountOfObjects]->objectType == "AABBFilled")
	{
		objectShaders.push_back(new Shader("Landscape/Shaders/cube"));
		CreateCube(glm::vec3(gameObjects[amountOfObjects]->objectScale));
	}//When creating check if it is a cube , if so create a shader and the cube
	//if (gameObjects[amountOfObjects]->objectType == "ISO")
	//{
		//tessellationControlShader = new Shader("Landscape/Shaders/tessellationControl",true);
		//tessellationEvaluationShader = new Shader("Landscape/Shaders/tessellationEvaluation",true);
		//CreateIsosahedron(glm::vec3(gameObjects[amountOfObjects]->objectScale));
	//}
	gameObjects.push_back(new Object());
	amountOfObjects++;
	gameObjects[amountOfObjects]->objectPosition = glm::vec3(1, 1, 1);
	gameObjects[amountOfObjects]->objectScale = 1.0f;
	gameObjects[amountOfObjects]->objectType = "default_object";
	//push back a new object ready with defautl values
}

void GameObject::DrawUI()
{
	ImGui::Begin("Object Creator");
	ImGui::InputFloat3("Object Position", glm::value_ptr(gameObjects[amountOfObjects]->objectPosition));
	ImGui::InputFloat("Object Scale", &gameObjects[amountOfObjects]->objectScale);
	const char* listbox_items_tex[] = { "Tile","grass","snow" };
	static int listbox_item_current_tex = 0;
	ImGui::ListBox("Object Texture", &listbox_item_current_tex, listbox_items_tex, sizeof(listbox_items_tex) / sizeof(listbox_items_tex[0]), 4);
	std::string fileDir = "Landscape/Textures/";
	std::string fileFormat = ".png";
	std::string textureToLoad = fileDir + listbox_items_tex[listbox_item_current_tex] + fileFormat;
	const char* listbox_items[] = {"AABBFilled"};
	static int listbox_item_current = 0;
	ImGui::ListBox("Object Type", &listbox_item_current, listbox_items, sizeof(listbox_items) / sizeof(listbox_items[0]), 4);
	if (ImGui::Button("Create Object"))
	{
		objectTextures.push_back(new aie::Texture(textureToLoad.c_str()));
		gameObjects[amountOfObjects]->objectType = listbox_items[listbox_item_current];
		Create();
		//Pass through texture and create the object
	}
	//Drawing a UI that has modifiable values to adjust position,size and textuire of the object
	ImGui::End();
}

void GameObject::CreateCube(glm::vec3 scale)
{
		Vertex verts[]  = 
		// front
		{ glm::vec4(-scale.x, -scale.y,  scale.z ,1),glm::vec2(0,0) ,
		{ glm::vec4(scale.x, -scale.y,  scale.z ,1),glm::vec2(1,0) },
		{ glm::vec4(scale.x,  scale.y,  scale.z ,1),glm::vec2(1,1) },
		{ glm::vec4(-scale.x,  scale.y,  scale.z ,1),glm::vec2(0,1) },
		// top				  			
		{ glm::vec4(-scale.x,  scale.y,  scale.z ,1),glm::vec2(0,0) },
		{ glm::vec4(scale.x,  scale.y,  scale.z ,1),glm::vec2(1,0) },
		{ glm::vec4(scale.x,  scale.y, -scale.z ,1),glm::vec2(1,1) },
		{ glm::vec4(-scale.x,  scale.y, -scale.z ,1),glm::vec2(0,1) },
		// back				  			
		{ glm::vec4(scale.x, -scale.y, -scale.z ,1),glm::vec2(0,0) },
		{ glm::vec4(-scale.x, -scale.y, -scale.z ,1),glm::vec2(1,0) },
		{ glm::vec4(-scale.x,  scale.y, -scale.z ,1),glm::vec2(1,1) },
		{ glm::vec4(scale.x,  scale.y, -scale.z ,1),glm::vec2(0,1) },
		// bottom			  			
		{ glm::vec4(-scale.x, -scale.y, -scale.z ,1),glm::vec2(0,0) },
		{ glm::vec4(scale.x, -scale.y, -scale.z ,1),glm::vec2(1,0) },
		{ glm::vec4(scale.x, -scale.y,  scale.z ,1),glm::vec2(1,1) },
		{ glm::vec4(-scale.x, -scale.y,  scale.z ,1),glm::vec2(0,1) },
		// left				  			
		{ glm::vec4(-scale.x, -scale.y, -scale.z ,1),glm::vec2(0,0) },
		{ glm::vec4(-scale.x, -scale.y,  scale.z ,1),glm::vec2(1,0) },
		{ glm::vec4(-scale.x,  scale.y,  scale.z ,1),glm::vec2(1,1) },
		{ glm::vec4(-scale.x,  scale.y, -scale.z ,1),glm::vec2(0,1) },
		// right			  			
		{ glm::vec4(scale.x, -scale.y,  scale.z ,1),glm::vec2(0,0) },
		{ glm::vec4(scale.x, -scale.y, -scale.z ,1),glm::vec2(1,0) },
		{ glm::vec4(scale.x,  scale.y, -scale.z ,1),glm::vec2(1,1) },
		{ glm::vec4(scale.x,  scale.y,  scale.z ,1),glm::vec2(0,1) },
	};

	unsigned char indices[] = {
		// front
		0,  1,  2,
		2,  3,  0,
		// top
		4,  5,  6,
		6,  7,  4,
		// back
		8,  9, 10,
		10, 11,  8,
		// bottom
		12, 13, 14,
		14, 15, 12,
		// left
		16, 17, 18,
		18, 19, 16,
		// right
		20, 21, 22,
		22, 23, 20
	};
	//Hard coding the verts and indices for a cube
	objectData.push_back(new BufferData());//push back an empty object
	objectData[amountOfObjects]->m_IndicesCount = sizeof(indices) / sizeof(unsigned char);//fill the indiceis

	// Generate the VAO and Bind bind it.
	// Our VBO (vertex buffer object) and IBO (Index Buffer Object) will be "grouped" with this VAO
	// other settings will also be grouped with the VAO. this is used so we can reduce draw calls in the render method.
	glGenVertexArrays(1, &objectData[amountOfObjects]->m_vao);
	glBindVertexArray(objectData[amountOfObjects]->m_vao);

	// Create our VBO and IBO.
	// Then tell Opengl what type of buffer they are used for
	// VBO a buffer in graphics memory to contains our vertices
	// IBO a buffer in graphics memory to contain our indices.
	// Then Fill the buffers with our generated data.
	// This is taking our verts and indices from ram, and sending them to the graphics card
	glGenBuffers(1, &objectData[amountOfObjects]->m_vbo);
	glGenBuffers(1, &objectData[amountOfObjects]->m_ibo);

	glBindBuffer(GL_ARRAY_BUFFER, objectData[amountOfObjects]->m_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objectData[amountOfObjects]->m_ibo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);//Position

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 4));//Tex coords

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

//void GameObject::CreateIsosahedron(glm::vec3 scale)
//{
//	const float verts[] = {
//		0.000f,  0.000f,  1.000f,
//		0.894f,  0.000f,  0.447f,
//		0.276f,  0.851f,  0.447f,
//		-0.724f,  0.526f,  0.447f,
//		-0.724f, -0.526f,  0.447f,
//		0.276f, -0.851f,  0.447f,
//		0.724f,  0.526f, -0.447f,
//		-0.276f,  0.851f, -0.447f,
//		-0.894f,  0.000f, -0.447f,
//		-0.276f, -0.851f, -0.447f,
//		0.724f, -0.526f, -0.447f,
//		0.000f,  0.000f, -1.000f };
//
//	const int Faces[] = {
//		2, 1, 0,
//		3, 2, 0,
//		4, 3, 0,
//		5, 4, 0,
//		1, 5, 0,
//		11, 6,  7,
//		11, 7,  8,
//		11, 8,  9,
//		11, 9,  10,
//		11, 10, 6,
//		1, 2, 6,
//		2, 3, 7,
//		3, 4, 8,
//		4, 5, 9,
//		5, 1, 10,
//		2,  7, 6,
//		3,  8, 7,
//		4,  9, 8,
//		5, 10, 9,
//		1, 6, 10 };
//	objectData.push_back(new BufferData());//push back an empty object
//	objectData[amountOfObjects]->m_IndicesCount = sizeof(Faces) / sizeof(Faces[0]);//fill the indiceis
//
//	glGenVertexArrays(1, &objectData[amountOfObjects]->m_vao);
//	glBindVertexArray(objectData[amountOfObjects]->m_vao);
//
//
//	glGenBuffers(1, &objectData[amountOfObjects]->m_vbo);
//	glGenBuffers(1, &objectData[amountOfObjects]->m_ibo);
//
//	glBindBuffer(GL_ARRAY_BUFFER, objectData[amountOfObjects]->m_vbo);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objectData[amountOfObjects]->m_ibo);
//
//	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Faces), Faces, GL_STATIC_DRAW);
//	// Create the VBO for positions:
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(verts), (void*)0);//Position
//	// Create the VBO for indices:
//
//
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//}
//void GameObject::DrawISO(glm::mat4 projectionView, std::vector<Light*> lightSources, Camera* m_camera)
//{
//	for (int i = 0; i < amountOfObjects; i++) {
//		if (gameObjects[i]->objectType == "ISO")
//		{
//			glPatchParameteri(GL_PATCH_VERTICES, 3);
//			glUseProgram(tessellationControlShader->m_program);
//			glBindVertexArray(objectData[i]->m_vao);
//			glDrawElements(GL_PATCHES, objectData[i]->m_IndicesCount, GL_UNSIGNED_INT, 0);
//			// unbind the VAO, cleaning up after ourselves
//			glBindVertexArray(0);
//			// Deactivate the shader
//			glUseProgram(0);
//
//			glUseProgram(tessellationEvaluationShader->m_program);
//			glBindVertexArray(objectData[i]->m_vao);
//			glDrawElements(GL_PATCHES, objectData[i]->m_IndicesCount, GL_UNSIGNED_INT, 0);
//			// unbind the VAO, cleaning up after ourselves
//			glBindVertexArray(0);
//			// Deactivate the shader
//			glUseProgram(0);
//		}
//	}
//}
void GameObject::DrawCube(glm::mat4 projectionView,std::vector<Light*> lightSources, Camera* m_camera)
{
	for (int i = 0; i < amountOfObjects; i++) {
		if (gameObjects[i]->objectType == "AABBFilled")
		{
			// Ask openGL to use our shader program
			glUseProgram(objectShaders[i]->m_program);

			glUniformMatrix4fv(glGetUniformLocation(objectShaders[i]->m_program, "projectionView"), 1, false, glm::value_ptr(projectionView));
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, objectTextures[i]->getHandle());
			glUniform1i(glGetUniformLocation(objectShaders[i]->m_program, "texture"), 0);
			//Pass in the projection view and texture to the shader

			// Bind VAO
			glBindVertexArray(objectData[i]->m_vao);

			// DRAW STUFF
			glDrawElements(GL_TRIANGLES, objectData[i]->m_IndicesCount, GL_UNSIGNED_BYTE, 0);

			// unbind the VAO, cleaning up after ourselves
			glBindVertexArray(0);

			// Deactivate the shader
			glUseProgram(0);
		}
	}
}

void GameObject::DrawAll(glm::mat4 projectionView,std::vector<Light*> lightSources,Camera* m_camera)
{
	for (int i = 0;i < amountOfObjects;i++)
	{
		if (gameObjects[i]->objectType == "AABBFilled")
		{
			DrawCube(projectionView,lightSources,m_camera);
		}//for each cube in the gameobjects, draw it
		//if (gameObjects[i]->objectType == "ISO")
		//{
		//	DrawISO(projectionView, lightSources, m_camera);
		//}

	}//Draw all gameobjects
}

void Object::Update(float d_time)//To be used if an operation needs to be peformed on certain/all gameobjects
{
	/*This is to be used to have generalised update functions for all gameobjects
		Such as rotating all objects every frame...*/
}
