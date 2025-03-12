#pragma once
#include <cmath>
#include "tgaimage.h"
#include "geometry.h"

enum RenderingMode {
	WIREFRAME,
	SOLID
};
const double EPSILON = 1e-5;

void drawScanLine(int x1, int x2, int y, TGAImage& framebuffer, TGAColor c);

void drawLine(vec2i v1, vec2i v2, TGAImage& framebuffer, TGAColor c);

void drawTriangleWireFrame(vec2i v1, vec2i v2, vec2i v3, TGAImage& framebuffer, TGAColor c);

int cross(int x1, int y1, int x2, int y2, int x3, int y3);

void drawTriangleSolid(vec2i v1, vec2i v2, vec2i v3, TGAImage& framebuffer, TGAColor c);