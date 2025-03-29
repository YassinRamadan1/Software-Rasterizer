#include <algorithm>
#include "Color.h"

Color::Color() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) {}

Color::Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}

Color::Color(TGAColor tga) {
	
	*this = this->toColor(tga);
}

void Color::init(float v, float alpha) {

	b = g = r = v, a = alpha;
}

void Color::set(float r, float g, float b, float a) {

	this->r = r, this->g = g, this->b = b, this->a = a;
}

Color Color::toColor(TGAColor tga) {

	float b = tga[0] / 255.0f;
	float g = tga[1] / 255.0f;
	float r = tga[2] / 255.0f;
	float a = tga[3] / 255.0f;
	return Color(r, g, b, a);
}

TGAColor Color::toTGAColor() const {
	TGAColor tga;
	tga[0] = static_cast<unsigned char>(std::clamp(b * 255.0f, 0.0f, 255.0f));
	tga[1] = static_cast<unsigned char>(std::clamp(g * 255.0f, 0.0f, 255.0f));
	tga[2] = static_cast<unsigned char>(std::clamp(r * 255.0f, 0.0f, 255.0f));

	//tga[3] = static_cast<unsigned char>(std::clamp(a * 255.0f, 0.0f, 255.0f));
	tga[3] = 255;
	return tga;
}

Color Color::operator+(const Color& other) const {
	return Color(r + other.r, g + other.g, b + other.b, a + other.a);
}

Color Color::operator-(const Color& other) const {
	return Color(r - other.r, g - other.g, b - other.b, a - other.a);
}

Color Color::operator*(float scalar) const {
	return Color(r * scalar, g * scalar, b * scalar, a * scalar);
}

float& Color::operator[](int i) {
	switch (i) {

	case 0:
		return r;
	case 1:
		return g;
	case 2:
		return b;
	case 3:
		return a;
	default:
		throw std::out_of_range("Color index out of range");
	}
}