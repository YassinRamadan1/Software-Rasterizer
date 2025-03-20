#pragma once

#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/matrix_inverse.hpp>

namespace utility {

	const float EPSILON = 1e-5;
	const float YAW = -90.0f;
	const float PITCH = 0.0f;
	const float SENSITIVITY = 1.0f;
	const float SPEED = 2.5f;
	const float ZOOM = 45.0f;
	enum Camera_Movement {
		FORWARD, BACKWARD, LEFT, RIGHT
	};

	float cross(glm::vec2 v1, glm::vec2 v2, glm::vec2 v3);

	glm::mat4 viewport(int x, int y, int w, int h, int near = 0, int far = 1);
	
	glm::mat4 perspectiveProjection(float fovy, float aspectRatio, float near, float far);

	glm::mat4 orthographicProjection(float fovy, float aspectRatio, float near, float far);

    class Transform {

public:
		float near, far;
        glm::mat4 model;
		glm::mat4 normalMatrix;
        glm::mat4 view;
        glm::mat4 projection;
		glm::mat4 viewport;

        // Default constructor initializes to identity matrices
        Transform() :
            model(1.0f),
			normalMatrix(1.0f),
            view(1.0f),
            projection(1.0f),
			viewport(1.0f) {}

        // Parameterized constructor
        Transform(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, const glm::mat4& viewport) :
            model(model),
            view(view),
            projection(projection),
			viewport(viewport){}

        // Convenience method to get the combined MVP matrix
        glm::mat4 getModelViewProjection() const {
            return projection * view * model;
        }
    };

	class Triangle {
	public:

		glm::vec4 position[3];
		glm::vec3 textureCoord[3];
		glm::vec4 color[3];
		glm::vec3 normal;

		Triangle() {

		}
		Triangle(glm::vec3 position1, glm::vec3 position2, glm::vec3 position3,
				glm::vec3 color1, glm::vec3 color2, glm::vec3 color3) {
			
			position[0] = glm::vec4(position1, 1.0f), position[1] = glm::vec4(position2, 1.0f),
				position[2] = glm::vec4(position3, 1.0f);
			color[0] = glm::vec4(color1, 255.0f), color[1] = glm::vec4(color2, 255.0f),
				color[2] = glm::vec4(color3, 255.0f);
		}

		Triangle(glm::vec3 position1, glm::vec3 position2, glm::vec3 position3) {
			
			position[0] = glm::vec4(position1, 1.0f), position[1] = glm::vec4(position2, 1.0f),
			position[2] = glm::vec4(position3, 1.0f);
		}

		Triangle(glm::vec3 position1, glm::vec3 position2, glm::vec3 position3, glm::vec2 textureCoord1,
				glm::vec2 textureCoord2, glm::vec2 textureCoord3) {
			
			position[0] = glm::vec4(position1, 1.0f), position[1] = glm::vec4(position2, 1.0f),
				position[2] = glm::vec4(position3, 1.0f), textureCoord[0] = glm::vec3(textureCoord1, 0.0f),
				textureCoord[1] = glm::vec3(textureCoord2, 0.0f), textureCoord[2] = glm::vec3(textureCoord3, 0.0f);
		}
	};

	class Camera {
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

		Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : m_Front(glm::vec3(0.0f, 0.0f, -1.0f)), m_Speed(SPEED), m_Sensitivity(SENSITIVITY), m_Zoom(ZOOM) {
			m_Position = position;
			m_WorldUp = up;
			m_Yaw = yaw;
			m_Pitch = pitch;
			UpdateCameraVectors();
		}

		void processKeyboard(Camera_Movement direction) {

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

		void processMouseMovement(float xOffset, float yOffset) {

			xOffset *= m_Sensitivity;
			yOffset *= m_Sensitivity;
			m_Yaw += xOffset;
			m_Pitch += yOffset;
			m_Pitch = m_Pitch > 89.0f ? 89.0f : m_Pitch < -89.0f ? -89.0f : m_Pitch;
			UpdateCameraVectors();
		}

		glm::mat4 getViewMatrix() {

			glm::vec3 forward = glm::normalize(-m_Front);
			glm::vec3 right = glm::normalize(glm::cross(m_WorldUp, forward));
			glm::vec3 up = glm::cross(forward, right);

			return  glm::transpose(glm::mat4(glm::vec4(right, 0.0f),
				glm::vec4(up, 0.0f),
				glm::vec4(forward, 0.0f),
				glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))) * glm::translate(glm::mat4(1.0f), -m_Position);
		}

	private:

		void UpdateCameraVectors() {

			m_Front.x = cos(glm::radians(m_Pitch)) * cos(glm::radians(m_Yaw));
			m_Front.y = sin(glm::radians(m_Pitch));
			m_Front.z = cos(glm::radians(m_Pitch)) * sin(glm::radians(m_Yaw));
			m_Front = glm::normalize(m_Front);
			m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
			m_Up = glm::cross(m_Right, m_Front);
		}
	};
}