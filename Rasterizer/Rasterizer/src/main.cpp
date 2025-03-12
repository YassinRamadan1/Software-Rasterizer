#include "tgaimage.h"
#include "model.h"

constexpr TGAColor white = { 255, 255, 255, 255 }; // attention, BGRA order
constexpr TGAColor green = { 0, 255,   0, 255 };
constexpr TGAColor red = { 0,   0, 255, 255 };
constexpr TGAColor blue = { 255, 128,  64, 255 };
constexpr TGAColor yellow = { 0, 200, 255, 255 };

int main(int argc, char** argv) {
    
    constexpr int width = 64;
    constexpr int height = 64;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    srand(time(0));

    model Mymodel("res/obj/african_head/african_head.obj", framebuffer);
    Mymodel.draw(SOLID);

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}
