#pragma once

#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/exponential.hpp>
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "tgaimage.h"

enum class RenderMode
{
	WIREFRAME,
	SOLID
};

enum class WrapMode
{
	REPEAT,
	MIRROR,
	CLAMP
};

enum class FilterMode
{
	NEAREST,
	BILINEAR,
	TRILINEAR
};

enum class BufferType
{
	DEPTH_BUFFER,
	COLOR_BUFFER
};

enum class WindingOrder
{
	CCW, CW, NONE
};

enum class NumberOfSamples
{
	ONE_SAMPLE = 1, TWO_SAMPLES = 2, FOUR_SAMPLES = 4, EIGHT_SAMPLES = 8, SIXTEEN_SAMPLES = 16
};

struct Face
{
	glm::ivec3 position;
	glm::ivec3 textureCoord;
	glm::ivec3 color;
	glm::ivec3 normal;
	Face() = default;
};

struct Triangle
{
	glm::vec4 position[3];
	glm::vec3 textureCoord[3];
	glm::vec3 color[3];
	glm::vec3 normal;

	Triangle() = default;
};

inline constexpr glm::vec2 MSAA1[1] =
{
		{0.5f, 0.5f},
};

inline constexpr glm::vec2 MSAA2[2] =
{
		{0.25f, 0.25f},
		{0.75f, 0.75f}
};

inline constexpr glm::vec2 MSAA4[4] =
{
		{0.625f, 0.125f},
		{0.125f, 0.375f},
		{0.375f, 0.875f},
		{0.875f, 0.625f}
};

inline constexpr glm::vec2 MSAA8[8] =
{
		{0.5625f, 0.1875f},
		{0.0625f, 0.3125f},
		{0.1875f, 0.9375f},
		{0.3125f, 0.0625f},
		{0.4375f, 0.5625f},
		{0.6875f, 0.6875f},
		{0.8125f, 0.4375f},
		{0.9375f, 0.8125f}
};

inline constexpr glm::vec2 MSAA16[16] =
{
		{0.53125f, 0.03125f},
		{0.59375f, 0.65625f},
		{0.65625f, 0.90625f},
		{0.71875f, 0.28125f},

		{0.03125f, 0.34375f},
		{0.09375f, 0.84375f},
		{0.15625f, 0.09375f},
		{0.21875f, 0.59375f},

		{0.28125f, 0.96875f},
		{0.34375f, 0.21875f},
		{0.40625f, 0.71875f},
		{0.46875f, 0.46875f},

		{0.78125f, 0.53125f},
		{0.84375f, 0.15625f},
		{0.90625f, 0.78125f},
		{0.96875f, 0.40625f}
};

namespace utility
{
	glm::mat4 perspectiveProjection(float fovy, float aspectRatio, float near, float far);

	glm::mat4 orthographicProjection(float fovy, float aspectRatio, float near, float far);

	glm::mat4 viewport(float x, float y, float width, float height, float near = 0.0f, float far = 1.0f);

	std::vector<std::string> split(std::string& line, char delimiter);

	const glm::vec2* getMSAASamples(NumberOfSamples numberOfSamples);

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

namespace clr
{
	glm::vec3 toColor(TGAColor tgaColor);

	TGAColor toTGAColor(glm::vec3 color);

	glm::vec3 gammaUncorrect(glm::vec3 color, float gamma);

	glm::vec3 gammaCorrect(glm::vec3 color, float gamma);
}