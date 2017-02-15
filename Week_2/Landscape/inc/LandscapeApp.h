#pragma once

#include "Application.h"
#include <glm/mat4x4.hpp>
#include "Camera.h"

class LandscapeApp : public aie::Application {
public:

	LandscapeApp();
	virtual ~LandscapeApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

	void DrawGrid();

	void LoadShader();
	void UnloadShader();

	void CreateCube();
	void DestroyCube();

	void CreateLandscape();
	void DestroyLandscape();

	void DrawLandscape();

protected:
	Camera *m_camera;

	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;

	unsigned int m_shader;

	unsigned int m_vertCount;
	unsigned int m_IndicesCount;

	unsigned int m_Vao;
	unsigned int m_Vbo;
	unsigned int m_Ibo;

	const int M_LAND_WIDTH = 64, M_LAND_DEPTH = 64;

	struct Vertex
	{
		glm::vec4 pos;
		glm::vec4 col;

		static void SetupVertexAttribPointers();
	};

};