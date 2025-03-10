#pragma once
#include <cmath>
#include "tgaimage.h"

void drawLine(int x1, int y1, int x2, int y2, TGAImage& framebuffer, TGAColor c);

void drawTriangleWireFrame(int x1, int y1, int x2, int y2, int x3, int y3, TGAImage& framebuffer, TGAColor c);