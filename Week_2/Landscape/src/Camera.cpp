#include "Camera.h"
#include <Input.h>


Camera::Camera(aie::Application* parent)
{
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
		parent->getWindowWidth() / (float)parent->getWindowHeight(),
		0.1f, 1000.f);
	app = parent;
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
		m_position += (m_moveSpeed  * m_speedMult ) * m_cameraLook * deltaTime;
		CalculateLook();
	}
	if (input->isKeyDown(aie::INPUT_KEY_S))
	{
		m_position -= (m_moveSpeed  * m_speedMult ) * m_cameraLook * deltaTime;
		CalculateLook();
	}

	if (input->isKeyDown(aie::INPUT_KEY_A))
	{
		m_position -= (m_moveSpeed  * m_speedMult) * strafe  * deltaTime;
		CalculateLook();
	}
	if (input->isKeyDown(aie::INPUT_KEY_D))
	{
		m_position += (m_moveSpeed  * m_speedMult) * strafe  * deltaTime;
		CalculateLook();
	}
	if (input->isKeyDown(aie::INPUT_KEY_Q))
	{
		m_position.y += (m_moveSpeed  * m_speedMult) * deltaTime;
		CalculateLook();
	}
	if (input->isKeyDown(aie::INPUT_KEY_E))
	{
		m_position.y -= (m_moveSpeed  * m_speedMult) * deltaTime;
		CalculateLook();
	}
	if (input->isKeyDown(aie::INPUT_KEY_LEFT_SHIFT))
	{
		m_speedMult = 5;
	}
	if (input->isKeyUp(aie::INPUT_KEY_LEFT_SHIFT))
	{
		m_speedMult = 1;
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

bool Camera::getFrustrumPlanes(glm::mat4 & transform, const GLfloat & xPos, const GLfloat & yPos, const GLfloat & zPos, const GLfloat & radius)
{
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

void Camera::CalculateLook()
{
	//some circle geometry maths to convert the viewing angle from
	//Yaw, Pitch and Roll into a single normalized vector
	
	//Protect against gimbal lock
	if (m_pitch >= 90)
	{
		m_pitch = 89.9f;
	}
	if (m_pitch <= -90)
	{
		m_pitch = -89.9f;
	}
	glm::vec3 look;
	look.x = glm::cos(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));
	look.y = glm::sin(glm::radians(m_pitch)) * glm::cos(glm::radians(m_roll));
	look.z = glm::sin(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));
	m_cameraLook = glm::normalize(look);

	CalculateView();
	CalculateProjection();
}

void Camera::CalculateView()
{
	m_viewMatrix = glm::lookAt(m_position, m_position + m_cameraLook, m_cameraUp);
}

void Camera::CalculateProjection()
{
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
		app->getWindowWidth() / (float)app->getWindowHeight(),
		0.1f, 1000.f);
}

const glm::vec3 & Camera::GetPos()
{
	return m_position;
}
