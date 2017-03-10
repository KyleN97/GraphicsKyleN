#pragma once
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Application.h"
#include <gl_core_4_4.h>
class aie::Application;
class Camera
{
public:
	Camera(aie::Application* parent);
	~Camera();

	void Update(float deltaTime);			//Update 
	void LookAt(glm::vec3 target);			//Look at a target vector
	void SetPosition(glm::vec3 position);	//set the position of the camera
	bool getFrustrumPlanes(glm::mat4& transform, const GLfloat &xPos, const GLfloat &yPos, const GLfloat &zPos, const GLfloat& radius) {
		// right side
		glm::vec4* planes;
		planes = new glm::vec4[6];
		planes[0] = glm::vec4(transform[0][3] - transform[0][0],
			transform[1][3] - transform[1][0],
			transform[2][3] - transform[2][0],
			transform[3][3] - transform[3][0]);
		GLfloat length = sqrtf(planes[0].x * planes[0].x + planes[0].y * planes[0].y + planes[0].z * planes[0].z);
		planes[0].x /= length;
		planes[0].y /= length;
		planes[0].z /= length;
		planes[0].w /= length;
		GLfloat distance = planes[0].x * xPos + planes[0].y * yPos + planes[0].z * zPos + planes[0].w;
		if (distance <= -radius)
		{
			return false; // Bounding sphere is completely outside the left plane
		}
		// left side
		planes[1] = glm::vec4(transform[0][3] + transform[0][0],
			transform[1][3] + transform[1][0],
			transform[2][3] + transform[2][0],
			transform[3][3] + transform[3][0]);
		length = sqrtf(planes[1].x * planes[1].x + planes[1].y * planes[1].y + planes[1].z * planes[1].z);
		planes[1].x /= length;
		planes[1].y /= length;
		planes[1].z /= length;
		planes[1].w /= length;
		distance = planes[1].x * xPos + planes[1].y * yPos + planes[1].z * zPos + planes[1].w;
		if (distance <= -radius)
		{
			return false; // Bounding sphere is completely outside the left plane
		}
		// top
		planes[2] = glm::vec4(transform[0][3] - transform[0][1],
			transform[1][3] - transform[1][1],
			transform[2][3] - transform[2][1],
			transform[3][3] - transform[3][1]);

		length = sqrtf(planes[2].x * planes[2].x + planes[2].y * planes[2].y + planes[2].z * planes[2].z);
		planes[2].x /= length;
		planes[2].y /= length;
		planes[2].z /= length;
		planes[2].w /= length;
		distance = planes[2].x * xPos + planes[2].y * yPos + planes[2].z * zPos + planes[2].w;
		if (distance <= -radius)
		{
			return false; // Bounding sphere is completely outside the left plane
		}
		// bottom
		planes[3] = glm::vec4(transform[0][3] + transform[0][1],
			transform[1][3] + transform[1][1],
			transform[2][3] + transform[2][1],
			transform[3][3] + transform[3][1]);
		length = sqrtf(planes[3].x * planes[3].x + planes[3].y * planes[3].y + planes[1].z * planes[3].z);
		planes[3].x /= length;
		planes[3].y /= length;
		planes[3].z /= length;
		planes[3].w /= length;
		distance = planes[3].x * xPos + planes[3].y * yPos + planes[3].z * zPos + planes[3].w;
		if (distance <= -radius)
		{
			return false; // Bounding sphere is completely outside the left plane
		}
		// far 
		planes[4] = glm::vec4(transform[0][3] - transform[0][2],
			transform[1][3] - transform[1][2],
			transform[2][3] - transform[2][2],
			transform[3][3] - transform[3][2]);
		length = sqrtf(planes[4].x * planes[4].x + planes[4].y * planes[4].y + planes[4].z * planes[4].z);
		planes[4].x /= length;
		planes[4].y /= length;
		planes[4].z /= length;
		planes[4].w /= length;
		distance = planes[4].x * xPos + planes[4].y * yPos + planes[4].z * zPos + planes[4].w;
		if (distance <= -radius)
		{
			return false; // Bounding sphere is completely outside the left plane
		}
		// near
		planes[5] = glm::vec4(transform[0][3] + transform[0][2],
			transform[1][3] + transform[1][2],
			transform[2][3] + transform[2][2],
			transform[3][3] + transform[3][2]);
		length = sqrtf(planes[5].x * planes[5].x + planes[5].y * planes[5].y + planes[5].z * planes[5].z);
		planes[5].x /= length;
		planes[5].y /= length;
		planes[5].z /= length;
		planes[5].w /= length;
		distance = planes[5].x * xPos + planes[5].y * yPos + planes[5].z * zPos + planes[5].w;
		if (distance <= -radius)
		{
			return false; // Bounding sphere is completely outside the left plane
		}
		return true;
	}

	const glm::mat4 &GetView() {
		return m_viewMatrix;
	}
	const glm::vec3& GetPos();
	const glm::mat4 &GetProjection() {
		return m_projectionMatrix;
	}
	glm::vec3 m_cameraLook;//the actual direction the camera is aiming

private:
	aie::Application* app;
	void CalculateLook();//gives us the vector of direction by using Yaw, Pitch and Roll
	void CalculateView();// sets up the view matrix based on our camera information
	void CalculateProjection();
	glm::mat4 m_viewMatrix;	//pass to OpenGL to know how the camera looks at the world
	glm::mat4 m_projectionMatrix;
	glm::vec3 m_cameraUp = glm::vec3(0.0f,1.0f,0.0f);//setting camera orientation so that the 				
	glm::vec3 m_position;//position of the camera						//y axis us always up

	float m_yaw = 0.0f;
	float m_pitch = 0.0f;
	float m_roll = 0.0f;//yaw = l/r rotation, pitch = up/down rotation, roll = twisting l/r e.g rolling l/r
	const float m_mouseSensitivity = 10.0f;//sensitivity of the mouse
	const float m_moveSpeed = 5.0f;
	float m_speedMult = 1;
	int m_lastMouseXPos = 0;
	int m_lastMouseYPos = 0; //record prev position to see how far it has moved


};

