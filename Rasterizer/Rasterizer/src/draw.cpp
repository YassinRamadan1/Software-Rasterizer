#include "draw.h"

void drawLine(vec2i v1, vec2i v2, TGAImage& framebuffer, TGAColor c) {

    bool isTransposed = false;
    if (abs(v2.x - v1.x) < abs(v2.y - v1.y)) { // transpose 
        std::swap(v1.x, v1.y);
        std::swap(v2.x, v2.y);
        isTransposed = true;
    }
    if (v2.x - v1.x < 0) { // draw from left to right
        std::swap(v1.x, v2.x);
        std::swap(v1.y, v2.y);
    }

    int dx = v2.x - v1.x, dy = v2.y - v1.y, x = v1.x, y = v1.y;

    if (isTransposed)
        framebuffer.set(y, x, c);
    else
        framebuffer.set(x, y, c);

    if (dy < 0) {

        int d = -dx - 2 * dy, d1 = -2 * dy, d2 = -2 * (dx + dy);

        while (x < v2.x) {
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
        while (x < v2.x) {
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

void drawTriangleWireFrame(vec2i v1, vec2i v2, vec2i v3, TGAImage& framebuffer, TGAColor c) {

    drawLine(v1, v2, framebuffer, c);
    drawLine(v2, v3, framebuffer, c);
    drawLine(v3, v1, framebuffer, c);
}

void drawScanLine(int x1, int x2, int y, TGAImage& framebuffer, TGAColor c) {

    if (x1 > x2)
        std::swap(x1, x2);
    while (x1 <= x2)
        framebuffer.set(x1++, y, c);
}

// calculating P1P2 x P1P3
int cross(int x1, int y1, int x2, int y2, int x3, int y3) {

    x2 -= x1, y2 -= y1, x3 -= x1, y3 -= y1;
    return x2 * y3 - x3 * y2;
}

void drawTriangleSolid(vec2i v1, vec2i v2, vec2i v3, TGAImage& framebuffer, TGAColor c) {

    // get the bounding box
    int xmin = std::min(v1.x, std::min(v2.x, v3.x)), ymin = std::min(v1.y, std::min(v2.y, v3.y)),
        xmax = std::max(v1.x, std::max(v2.x, v3.x)), ymax = std::max(v1.y, std::max(v2.y, v3.y));
    
    double area = cross(v1.x, v1.y, v2.x, v2.y, v3.x, v3.y);

    if (area < 1.0f)
        return;
    // it is an instruction to OpenMP to parallelize the loop since it is perfectly parallel
    #pragma omp parallel for
    for (int y = ymin; y < ymax + 1; y++) {
        for (int x = xmin; x < xmax + 1; x++) {
            
            // get the barycentric coordinates u * P1 + v * P2 + w * P3
            // watch out for orientation!
            double u = cross(x, y, v2.x, v2.y, v3.x, v3.y) / area,
                v = cross(x, y, v3.x, v3.y, v1.x, v1.y) / area,
                w = 1 - u - v;
                
            if (u < 0.0f || v < 0.0f || w < 0.0f)
                continue;
            framebuffer.set(x, y, c);
            
        }
    }
}
