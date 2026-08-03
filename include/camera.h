#pragma once

#include <glm/glm.hpp>
#include <glm/exponential.hpp>
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/matrix_inverse.hpp>

enum Camera_Movement {
	FORWARD, BACKWARD, LEFT, RIGHT
};

const float EPSILON = 1e-5;
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SENSITIVITY = 1.0f;
const float SPEED = 2.5f;
const float ZOOM = 45.0f;

class Camera
{
public:
	glm::vec3 m_Position;
	glm::vec3 m_Front;
	glm::vec3 m_Up;
	glm::vec3 m_Right;
	glm::vec3 m_WorldUp;

	float m_Speed;
	float m_Sensitivity;
	float m_Zoom;
	float m_Yaw;
	float m_Pitch;

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);

	void processKeyboard(Camera_Movement direction);

	void processMouseMovement(float xOffset, float yOffset);

	glm::mat4 getViewMatrix();

private:
	void updateCameraVectors();
};