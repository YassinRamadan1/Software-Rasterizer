#pragma once
#include <iostream>
#include <cmath>

template<typename T>
class Vec2 {

public:

	union {
		struct {
			T x, y;
		};

		struct {
			T u, v;
		};

		T raw[2];
	};

	Vec2() : x(0), y(0) {}
	Vec2(T _v) : x(_v), y(_v) {}
	Vec2(T _x, T _y) : x(_x), y(_y){}

	Vec2<T> operator+(const Vec2<T>& other) const {
		return Vec2<T>(x + other.x, y + other.y);
	}
	
	Vec2<T> operator-(const Vec2<T>& other) const {
		return Vec2<T>(x - other.x, y - other.y);
	}

	T operator*(const Vec2<T>& other) const {
		return x * other.x + y * other.y;
	}

	Vec2<T> operator*(T v) const {
		return Vec2<T>(x * v, y * v);
	}

	template<typename T>
	friend std::ostream& operator<<(std::ostream& out, Vec2<T> v);
};

template<typename T>
std::ostream& operator<<(std::ostream& out, Vec2<T> v) {
	out << "(" << v.x << ", " << v.y << ")\n";
	return out;
}

typedef Vec2<float> vec2;
typedef Vec2<double> vec2d;
typedef Vec2<int> vec2i;
typedef Vec2<unsigned int> vec2ui;

template<typename T>
class Vec3 {

public:

	union {
		struct {
			T x, y, z;
		};

		struct {
			T u, v, w;
		};

		T raw[3];
	};

	Vec3() : x(0), y(0), z(0) {}
	Vec3(T _v) : x(_v), y(_v), z(_v) {}
	Vec3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}

	Vec3<T> operator+(const Vec3<T>& other) const {
		return Vec3<T>(x + other.x, y + other.y, z + other.z);
	}

	Vec3<T> operator-(const Vec3<T>& other) const {
		return Vec3<T>(x - other.x, y - other.y, z - other.z);
	}
	
	Vec3<T> operator^(const Vec3<T>& other) const {
		return Vec3<T>(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
	}

	T operator*(const Vec3<T>& other) const {
		return x * other.x + y * other.y + z * other.z;
	}

	Vec3<T> operator*(T v) const {
		return Vec3<T>(x * v, y * v, z * v);
	}

	double norm() const {
		return std::sqrtl(x * x + y * y + z * z);
	}

	Vec3<T>& normalize() {
		
		*this = (*this) * (1 / (this->norm()));
		return *this;
	}

	template<typename T>
	friend std::ostream& operator<<(std::ostream& out, Vec3<T> v);
};

template<typename T>
std::ostream& operator<<(std::ostream& out, Vec3<T> v) {
	out << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
	return out;
}

typedef Vec3<float> vec3;
typedef Vec3<double> vec3d;
typedef Vec3<int> vec3i;
typedef Vec3<unsigned int> vec3ui;