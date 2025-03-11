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

// calculating P1P2 x P1P3
int cross(int x1, int y1, int x2, int y2, int x3, int y3) {

    x2 -= x1, y2 -= y1, x3 -= x1, y3 -= y1;
    return x2 * y3 - x3 * y2;
}

void drawTriangleFilled(int x1, int y1, int x2, int y2, int x3, int y3, TGAImage& framebuffer, TGAColor c) {

    // get the bounding box
    int xmin = std::min(x1, std::min(x2, x3)), ymin = std::min(y1, std::min(y2, y3)),
        xmax = std::max(x1, std::max(x2, x3)), ymax = std::max(y1, std::max(y2, y3));

    
    double area = cross(x1, y1, x2, y2, x3, y3);
    #pragma omp parallel for
    for (int y = ymin; y < ymax + 1; y++) {
        for (int x = xmin; x < xmax + 1; x++) {
            
            // get the barycentric coordinates u * P1 + v * P2 + w * P3
            // watch out for orientation!
            double u = cross(x, y, x2, y2, x3, y3) / area,
                v = cross(x, y, x3, y3, x1, y1) / area,
                w = cross(x, y, x1, y1, x2, y2) / area;
                
            if (u < 0.0f || v < 0.0f || w < 0.0f)
                continue;
            framebuffer.set(x, y, c);
            
        }
    }
}
