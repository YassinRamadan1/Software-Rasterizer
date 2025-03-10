#include "draw.h"

void drawLine(int x1, int y1, int x2, int y2, TGAImage& framebuffer, TGAColor c) {

    bool isTransposed = false;
    if (abs(x2 - x1) < abs(y2 - y1)) { // transpose 
        std::swap(x1, y1);
        std::swap(x2, y2);
        isTransposed = true;
    }
    if (x2 - x1 < 0) { // draw from left to right
        std::swap(x1, x2);
        std::swap(y1, y2);
    }

    int dx = x2 - x1, dy = y2 - y1, x = x1, y = y1;

    if (isTransposed)
        framebuffer.set(y, x, c);
    else
        framebuffer.set(x, y, c);

    if (dy < 0) {

        int d = -dx - 2 * dy, d1 = -2 * dy, d2 = -2 * (dx + dy);

        while (x < x2) {
            x++;
            if (d < 0)
                d += d1;
            else
                y--, d += d2;
            if (isTransposed)
                framebuffer.set(y, x, c);
            else
                framebuffer.set(x, y, c);
        }
    }
    else {

        int d = dx - 2 * dy, d1 = -2 * dy, d2 = 2 * (dx - dy);
        while (x < x2) {
            x++;
            if (d > 0)
                d += d1;
            else
                y++, d += d2;
            if (isTransposed)
                framebuffer.set(y, x, c);
            else
                framebuffer.set(x, y, c);
        }
    }
}

void drawTriangleWireFrame(int x1, int y1, int x2, int y2, int x3, int y3, TGAImage& framebuffer, TGAColor c) {

    drawLine(x1, y1, x2, y2, framebuffer, c);
    drawLine(x2, y2, x3, y3, framebuffer, c);
    drawLine(x3, y3, x1, y1, framebuffer, c);
}