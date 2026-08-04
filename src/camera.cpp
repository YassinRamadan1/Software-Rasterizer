#include "camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
	: m_Front(glm::vec3(0.0f, 0.0f, -1.0f)), m_Speed(SPEED), m_Sensitivity(SENSITIVITY), m_Zoom(ZOOM)
{
	m_Position = position;
	m_WorldUp = up;
	m_Yaw = yaw;
	m_Pitch = pitch;
	updateCameraVectors();
}

void Camera::processKeyboard(Camera_Movement direction)
{
	float velocity = m_Speed;

	if (direction == FORWARD)
		m_Position += velocity * m_Front;
	if (direction == BACKWARD)
		m_Position -= velocity * m_Front;
	if (direction == LEFT)
		m_Position -= velocity * m_Right;
	if (direction == RIGHT)
		m_Position += velocity * m_Right;
}

void Camera::processMouseMovement(float xOffset, float yOffset)
{
	xOffset *= m_Sensitivity;
	yOffset *= m_Sensitivity;
	m_Yaw += xOffset;
	m_Pitch += yOffset;
	m_Pitch = m_Pitch > 89.0f ? 89.0f : m_Pitch < -89.0f ? -89.0f : m_Pitch;
	updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix()
{
	glm::vec3 forward = glm::normalize(-m_Front);
	glm::vec3 right = glm::normalize(glm::cross(m_WorldUp, forward));
	glm::vec3 up = glm::cross(forward, right);

	return  glm::transpose(glm::mat4(glm::vec4(right, 0.0f),
		glm::vec4(up, 0.0f),
		glm::vec4(forward, 0.0f),
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))) * glm::translate(glm::mat4(1.0f), -m_Position);
}

void Camera::updateCameraVectors()
{
	m_Front.x = cos(glm::radians(m_Pitch)) * cos(glm::radians(m_Yaw));
	m_Front.y = sin(glm::radians(m_Pitch));
	m_Front.z = cos(glm::radians(m_Pitch)) * sin(glm::radians(m_Yaw));
	m_Front = glm::normalize(m_Front);
	m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
	m_Up = glm::cross(m_Right, m_Front);
}