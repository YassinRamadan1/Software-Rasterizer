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
#include "Color.h"

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

enum class BUFFER_TYPE {
	DEPTH_BUFFER,
	COLOR_BUFFER
};

struct Face {

	int position[3];
	int texture[3];
	int color[3];
	int normal[3];
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

class Triangle {
public:

	glm::vec4 position[3];
	glm::vec3 textureCoord[3];
	Color color[3];
	glm::vec3 normal;

	Triangle() {}
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

	glm::mat4 perspectiveProjection(float fovy, float aspectRatio, float near, float far);

	glm::mat4 orthographicProjection(float fovy, float aspectRatio, float near, float far);

	std::vector<std::string> split(std::string& line, char delimiter);

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

	template<typename T, typename T2, size_t dp>
	class FixedPoint {

		static FixedPoint create(T val) {

			FixedPoint fixed;
			fixed.value = val;
			return fixed;
		}

	public:

		T value = T(0);

		FixedPoint() {}

		FixedPoint(double val) {

			set(val);
		}
		
		void setULP() {
			value = 1;
		}

		void set(double val) {

			value = (val * (1 << dp) + ((val < 0) ? -0.5 : 0.5));
		}

		operator double() const {

			return double(value) / (1 << dp);
		}

		FixedPoint operator-() {

			return create(-value);
		}

		FixedPoint operator+(FixedPoint& other) {

			return create(value + other.value);
		}

		FixedPoint& operator+=(FixedPoint& other) {

			value += other.value;
			return *this;
		}

		FixedPoint operator-(FixedPoint& other) {

			return create(value - other.value);
		}

		FixedPoint& operator-=(FixedPoint& other) {

			value -= other.value;
			return *this;
		}

		FixedPoint operator*(FixedPoint& other) {

			return create((T2(value) * T2(other.value)) >> dp);
		}

		FixedPoint& operator*=(FixedPoint& other) {

			value = (T2(value) * T2(other.value)) >> dp;
			return *this;
		}

		FixedPoint operator/(FixedPoint& other) {

			if (other.value == 0) {
				throw std::runtime_error("Division by zero");
			}
			return create(T2(value << dp) / T2(other.value));
		}

		FixedPoint& operator/=(FixedPoint& other) {

			if (other.value == 0) {
				throw std::runtime_error("Division by zero");
			}
			value = T2(value << dp) / T2(other.value);
			return *this;
		}

		bool operator==(FixedPoint& other) {
			return value == other.value;
		}

		bool operator<(FixedPoint& other) {
			return value < other.value;
		}

		bool operator<=(FixedPoint& other) {
			return value <= other.value;
		}
	};

	template<typename T>
	class vec2 {

	public:
		union {
			struct {
				T x, y;
			};
			struct {
				T r, g;
			};
			struct {
				T s, t;
			};
			T data[2];
		};

		vec2() : x(0), y(0) {}
		vec2(T x, T y) : x(x), y(y) {}

		T& operator[](size_t index) {
			return data[index];
		}

		const T& operator[](size_t index) const {
			return data[index];
		}

		vec2 operator+(const vec2& other) const {

			return vec2(x + other.x, y + other.y);
		}

		vec2 operator-(const vec2& other) const {

			return vec2(x - other.x, y - other.y);
		}

		vec2 operator*(T val) const {

			return vec3(val * x, val * y);
		}
	};

	template<typename T>
	class vec3 {

	public:
		union {
			struct {
				T x, y, z;
			};
			struct {
				T r, g, b;
			};
			struct {
				T s, t, p;
			};
			T data[3];
		};

		vec3() : x(0), y(0), z(0) {}
		vec3(T x, T y, T z) : x(x), y(y), z(z) {}

		T& operator[](size_t index) {
			return data[index];
		}

		const T& operator[](size_t index) const {
			return data[index];
		}

		vec3 operator+(const vec3& other) const {

			return vec3(x + other.x, y + other.y, z + other.z);
		}

		vec3 operator-(const vec3& other) const {

			return vec3(x - other.x, y - other.y, z - other.z);
		}

		vec3 operator*(T val) const {

			return vec3(val * x, val * y, val * z);
		}
	};

	template<typename T>
	class vec4 {

	public:
		union {
			struct {
				T x, y, z, w;
			};
			struct {
				T r, g, b, a;
			};
			struct {
				T s, t, p, q;
			};
			T data[4];
		};

		vec4() : x(0), y(0), z(0), w(0) {}
		vec4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}

		T& operator[](size_t index) {
			return data[index];
		}

		const T& operator[](size_t index) const {
			return data[index];
		}

		vec4 operator+(const vec4& other) const {

			return vec4(x + other.x, y + other.y, z + other.z, w + other.w);
		}

		vec4 operator-(const vec4& other) const {

			return vec4(x - other.x, y - other.y, z - other.z, w - other.w);
		}

		vec4 operator*(T val) const {

			return vec4(val * x, val * y, val * z, val * w);
		}
	};

	template<typename T>
	T dot(vec2<T>& a, vec2<T>& b) {

		return a.x * b.x + a.y * b.y;
	}

	template<typename T>
	T dot(vec3<T>& a, vec3<T>& b) {

		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	template<typename T>
	T dot(vec4<T>& a, vec4<T>& b) {

		return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	}

	template<typename T>
	T cross(vec2<T>& v0, vec2<T>& v1, vec2<T>& v2) {

		return (v1.x - v0.x) * (v2.y - v0.y) - (v1.y - v0.y) * (v2.x - v0.x);
	}
}

using fp15_16 = utility::FixedPoint<int32_t, int64_t, 16>;
using fp22_9 = utility::FixedPoint<int32_t, int64_t, 9>;
using fp46_16 = utility::FixedPoint<int64_t, int64_t, 16>;