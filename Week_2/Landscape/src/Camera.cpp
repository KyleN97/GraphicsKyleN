#include "Camera.h"
#include <Input.h>


Camera::Camera()
{

}


Camera::~Camera()
{

}

void Camera::Update(float deltaTime)
{
	//Rotate camera bases on mouse movement
	aie::Input * input = aie::Input::getInstance();// get a pointer to the input manager
	if (input->isMouseButtonDown(aie::INPUT_MOUSE_BUTTON_RIGHT))
	{
		float rotationAmount = m_mouseSensitivity * deltaTime;
		int mouseX, mouseY;		//mouse x and y co-ords from input
		float xOffset, yOffset; //how far we want to rotate the camera in these axes
		input->getMouseXY(&mouseX, &mouseY);//get mouse location and place it in our variables

		//find out how far the mouse has moved since last frame
		xOffset = (mouseX - m_lastMouseXPos) * rotationAmount;
		yOffset = (mouseY - m_lastMouseYPos) * rotationAmount;

		//save last mouse position
		m_lastMouseXPos = mouseX;
		m_lastMouseYPos = mouseY;

		//use these values in out Yaw and Pitch values(so we can rotate that far)
		m_yaw += xOffset;
		m_pitch += yOffset;


		//calculate the new cameraLook vector
		CalculateLook();


	}
	else {
		//track the last position of the mouse regardless of wether we're clicking
		input->getMouseXY(&m_lastMouseXPos, &m_lastMouseYPos);
	}
	glm::vec3 strafe = glm::normalize(glm::cross(m_cameraLook, m_cameraUp));
	if (input->isKeyDown(aie::INPUT_KEY_W))
	{
		m_position += m_moveSpeed * m_cameraLook * deltaTime;
		CalculateLook();
	}
	if (input->isKeyDown(aie::INPUT_KEY_S))
	{
		m_position -= m_moveSpeed * m_cameraLook * deltaTime;
		CalculateLook();
	}
	if (input->isKeyDown(aie::INPUT_KEY_A))
	{
		m_position -= m_moveSpeed * strafe  * deltaTime;
		CalculateLook();
	}
	if (input->isKeyDown(aie::INPUT_KEY_D))
	{
		m_position += m_moveSpeed * strafe  * deltaTime;
		CalculateLook();
	}
	if (input->isKeyDown(aie::INPUT_KEY_Q))
	{
		m_position.y += m_moveSpeed *deltaTime;
		CalculateLook();
	}
	if (input->isKeyDown(aie::INPUT_KEY_E))
	{
		m_position.y -= m_moveSpeed *deltaTime;
		CalculateLook();
	}
}

void Camera::LookAt(glm::vec3 target)
{
	glm::vec3 look = glm::normalize(target - m_position);//create a vector which is the direction to the target from our position
	m_pitch = glm::degrees(glm::asin(look.y));
	m_yaw = glm::degrees(atan2(look.y,look.x));
	CalculateLook();
}

void Camera::SetPosition(glm::vec3 position)
{
	m_position = position;
	CalculateView();
}

void Camera::CalculateLook()
{
	//some circle geometry maths to convert the viewing angle from
	//Yaw, Pitch and Roll into a single normalized vector

	glm::vec3 look;
	look.x = glm::cos(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));
	look.y = glm::sin(glm::radians(m_pitch)) * glm::cos(glm::radians(m_roll));
	look.z = glm::sin(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));
	m_cameraLook = glm::normalize(look);

	CalculateView();
}

void Camera::CalculateView()
{
	m_viewMatrix = glm::lookAt(m_position, m_position + m_cameraLook, m_cameraUp);
}
