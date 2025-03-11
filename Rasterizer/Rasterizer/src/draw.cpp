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

void drawScanLine(int x1, int x2, int y, TGAImage& framebuffer, TGAColor c) {

    if (x1 > x2)
        std::swap(x1, x2);
    while (x1 <= x2)
        framebuffer.set(x1++, y, c);
}

void drawTriangleFilled(int x1, int y1, int x2, int y2, int x3, int y3, TGAImage& framebuffer, TGAColor c) {

    // To ensure that p1 > p2 > p3 in the y direction
    if (y1 < y2)
        std::swap(x1, x2), std::swap(y1, y2);
    if (y1 < y3)
        std::swap(x1, x3), std::swap(y1, y3);
    if (y2 < y3)
        std::swap(x2, x3), std::swap(y2, y3);

    // Draw the Top part of the triangle
    int dx1 = x2 - x1, dy1 = y2 - y1, dx2 = x3 - x1, dy2 = y3 - y1,
        start = x1, end = x1, y = y1;

    framebuffer.set(start, y, c);

    if (y1 == y2)
        drawScanLine(x1, x2, y2, framebuffer, c);

    while (y > y2) {
        y--;

        // getting the next point for line 1
        start = std::round(x1 + 1.0f * dx1 * (y - y1) / dy1);

        // getting  the next point for line 2
        end = std::round(x1 + 1.0f * dx2 * (y - y1) / dy2);

        drawScanLine(start, end, y, framebuffer, c);
    }

    // Draw the bottom part of the triangle
    dx1 = x3 - x2, dy1 = y3 - y2;


    while (y > y3) {
        y--;

        // getting the next point for the new line 3 from p2 to p3
        start = std::round(x2 + 1.0f * dx1 * (y - y2) / dy1);

        // still getting the next point for line 2
        end = std::round(x1 + 1.0f * dx2 * (y - y1) / dy2);

        drawScanLine(start, end, y, framebuffer, c);
    }

}
