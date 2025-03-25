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

#include "tgaimage.h"

enum class RenderMode {
	WIREFRAME,
	SOLID
};

enum class ProjectionMode {
	PERSPECTIVE,
	ORTHOGRAPHIC
};

enum class AttributeMode {
	COLOR,
	TEXTURE
};

enum class WrapMode {
	REPEAT,
	MIRROR,
	CLAMP
};

enum class FilterMode {
	NEAREST,
	BILINEAR,
	TRILINEAR
};

struct Face {

	int position[3];
	int texture[3];
	int color[3];
	Face() {}

	Face(int position0, int position1, int position2, int tex0, int tex1, int tex2) {
		position[0] = position0, position[1] = position1, position[2] = position2,
			texture[0] = tex0, texture[1] = tex1, texture[2] = tex2;
	}

	Face(int position0, int position1, int position2, int c0, int c1, int c2, int tex0, int tex1, int tex2) {
		position[0] = position0, position[1] = position1, position[2] = position2,
			color[0] = c0, color[1] = c1, color[2] = c2,
			texture[0] = tex0, texture[1] = tex1, texture[2] = tex2;
	}
};

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

	float cross(glm::vec2 v1, glm::vec2 v2, glm::vec2 v3) {
		return (v2.x - v1.x) * (v3.y - v1.y) - (v2.y - v1.y) * (v3.x - v1.x);
	}

	glm::mat4 viewport(int x, int y, int w, int h, int near = 0, int far = 1) {

		return glm::mat4(w / 2.0f, 0.0f, 0.0f, 0.0f,
			0.0f, h / 2.0f, 0.0f, 0.0f,
			0.0f, 0.0f, (far - near) / 2.0f, 0.0f,
			x + w / 2.0f, y + h / 2.0f, (far + near) / 2.0f, 1.0f);
	}

	glm::mat4 perspectiveProjection(float fovy, float aspectRatio, float near, float far) {

		float t = near * glm::tan(fovy / 2.0f), r = t * aspectRatio;

		return glm::perspective(fovy, aspectRatio, near, far);
	}

	glm::mat4 orthographicProjection(float fovy, float aspectRatio, float near, float far) {

		float t = near * glm::tan(fovy / 2.0f), r = t * aspectRatio;

		return glm::ortho(-r, r, -t, t, near, far);
	}

	class Color {

	public:
		float r, g, b, a;

		Color() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) {}
		Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}

		Color(TGAColor& tga) {
			b = tga[0] / 255.0f;
			g = tga[1] / 255.0f;
			r = tga[2] / 255.0f;
			a = tga[3] / 255.0f;
		}
		
		void init(float v, float alpha = 1.0f) {

			b = g = r = v, a = alpha;
		}

		void set(float r, float g, float b, float a = 1.0f) {

			this->r = r, this->g = g, this->b = b, this->a = a;
		}
		Color toColor(TGAColor tga) {

			float b = tga[0] / 255.0f;
			float g = tga[1] / 255.0f;
			float r = tga[2] / 255.0f;
			float a = tga[3] / 255.0f;
			return Color(r, g, b, a);
		}

		TGAColor toTGAColor() const {
			TGAColor tga;
			tga[0] = static_cast<unsigned char>(std::clamp(b * 255.0f, 0.0f, 255.0f));
			tga[1] = static_cast<unsigned char>(std::clamp(g * 255.0f, 0.0f, 255.0f));
			tga[2] = static_cast<unsigned char>(std::clamp(r * 255.0f, 0.0f, 255.0f));
			
			//tga[3] = static_cast<unsigned char>(std::clamp(a * 255.0f, 0.0f, 255.0f));
			tga[3] = 255;
			return tga;
		}

		Color operator+(const Color& other) const {
			return Color(r + other.r, g + other.g, b + other.b, a + other.a);
		}

		Color operator-(const Color& other) const {
			return Color(r - other.r, g - other.g, b - other.b, a - other.a);
		}

		Color operator*(float scalar) const {
			return Color(r * scalar, g * scalar, b * scalar, a * scalar);
		}

		float& operator[](int i) {
			switch (i) {
			
				case 0:
					return r;
				case 1:
					return g;
				case 2:
					return b;
				case 3:
					return a;
			}
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


class Triangle {
public:

	glm::vec4 position[3];
	glm::vec3 textureCoord[3];
	utility::Color color[3];
	glm::vec3 normal;

	Triangle() {}
};