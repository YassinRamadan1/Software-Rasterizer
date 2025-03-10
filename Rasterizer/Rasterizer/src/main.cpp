#include <cmath>
#include "tgaimage.h"

constexpr TGAColor white = { 255, 255, 255, 255 }; // attention, BGRA order
constexpr TGAColor green = { 0, 255,   0, 255 };
constexpr TGAColor red = { 0,   0, 255, 255 };
constexpr TGAColor blue = { 255, 128,  64, 255 };
constexpr TGAColor yellow = { 0, 200, 255, 255 };



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

int main(int argc, char** argv) {
    
    constexpr int width = 64;
    constexpr int height = 64;
    TGAImage framebuffer(width, height, TGAImage::RGB);
    /*
    int ax = 7, ay = 3;
    int bx = 12, by = 37;
    int cx = 62, cy = 53;

    
    drawLine(ax, ay, cx, cy, red);
    drawLine(cx, cy, bx, by, green);
    drawLine(bx, by, ax, ay, blue);

    framebuffer.set(ax, ay, white);
    framebuffer.set(bx, by, white);
    framebuffer.set(cx, cy, white);*/

    std::srand(std::time({}));
    for (int i = 0; i < (1 << 24); i++) {
        int ax = rand() % width, ay = rand() % height;
        int bx = rand() % width, by = rand() % height;
        unsigned char b = rand() % 255, g = rand() % 255, r = rand() % 255, a = rand() % 255;
        drawLine(ax, ay, bx, by, framebuffer, {b, g, r, a});
    }

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}
