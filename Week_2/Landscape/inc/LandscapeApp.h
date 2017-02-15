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

protected:
	Camera *m_camera;

	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;

	unsigned int m_shader;

	unsigned int m_cubeIndicesCount;

	unsigned int m_cubeVao;
	unsigned int m_cubeVbo;
	unsigned int m_cubeIbo;

	struct Vertex
	{
		glm::vec4 pos;
		glm::vec4 col;

		static void SetupVertexAttribPointers();
	};

};