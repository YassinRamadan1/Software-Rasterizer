#pragma once

#include "tgaimage.h"

class Color {

public:
	
	float r, g, b, a;

	Color();
	
	Color(float r, float g, float b, float a = 1.0f);
	
	Color(TGAColor tga);

	void init(float v, float alpha = 1.0f);

	void set(float r, float g, float b, float a = 1.0f);

	Color toColor(TGAColor tga);

	TGAColor toTGAColor() const;

	Color operator+(const Color& other) const;

	Color operator-(const Color& other) const;

	Color operator*(float scalar) const;

	float& operator[](int i);
};
