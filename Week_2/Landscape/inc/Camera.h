#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>
class Camera
{
public:
	Camera();
	~Camera();

	void Update(float deltaTime);			//Update 
	void LookAt(glm::vec3 target);			//Look at a target vector
	void SetPosition(glm::vec3 position);	//set the position of the camera
	
	const glm::mat4 &GetView() {
		return m_viewMatrix;
	}
		const glm::vec3& GetPos();

private:
	void CalculateLook();//gives us the vector of direction by using Yaw, Pitch and Roll
	void CalculateView();// sets up the view matrix based on our camera information
	glm::mat4 m_viewMatrix;	//pass to OpenGL to know how the camera looks at the world

	glm::vec3 m_cameraLook;//the actual direction the camera is aiming
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

