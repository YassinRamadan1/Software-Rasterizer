#pragma once
#include <cmath>
#include "tgaimage.h"
#include "utility.h"

enum RenderingMode {
	WIREFRAME,
	SOLID
};

void drawScanLine(int x1, int x2, int y, TGAImage& framebuffer, TGAColor c);

void drawLine(glm::ivec2 v1, glm::ivec2 v2, TGAImage& framebuffer, TGAColor c);

void drawTriangleWireFrame(glm::ivec2 v1, glm::ivec2 v2, glm::ivec2 v3, TGAImage& framebuffer, TGAColor c);

std::vector<utility::Triangle> clipFarPlane(utility::Triangle& triangle);

std::vector<utility::Triangle> clipNearPlane(utility::Triangle& triangle);

std::vector<utility::Triangle> clip(utility::Triangle& triangle);

void drawTriangleSolid(utility::Triangle& triangle, TGAImage& tex, float* zBuffer, TGAImage& framebuffer, bool projectionType, float n, float f);

void rasterize(utility::Triangle triangle, utility::Transform& transform, TGAImage& texture, float* zBuffer, TGAImage& framebuffer);
