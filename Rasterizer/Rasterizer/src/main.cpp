#include <cmath>
#include "tgaimage.h"

constexpr TGAColor white = { 255, 255, 255, 255 }; // attention, BGRA order
constexpr TGAColor green = { 0, 255,   0, 255 };
constexpr TGAColor red = { 0,   0, 255, 255 };
constexpr TGAColor blue = { 255, 128,  64, 255 };
constexpr TGAColor yellow = { 0, 200, 255, 255 };

constexpr int width = 64;
constexpr int height = 64;
TGAImage framebuffer(width, height, TGAImage::RGB);

void drawLine(int x1, int y1, int x2, int y2, TGAColor c) {

    for (double t = 0.0f; t < 1.0f; t += 0.02) {
        int x = round(x1 + t * (x2 - x1));
        int y = round(y1 + t * (y2 - y1));
        framebuffer.set(x, y, c);
    }
}

int main(int argc, char** argv) {
    

    int ax = 7, ay = 3;
    int bx = 12, by = 37;
    int cx = 62, cy = 53;

    drawLine(ax, ay, bx, by, white);
    framebuffer.set(cx, cy, white);

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}
