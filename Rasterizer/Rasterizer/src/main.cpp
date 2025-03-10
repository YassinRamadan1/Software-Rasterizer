#include "tgaimage.h"
#include "model.h"

constexpr TGAColor white = { 255, 255, 255, 255 }; // attention, BGRA order
constexpr TGAColor green = { 0, 255,   0, 255 };
constexpr TGAColor red = { 0,   0, 255, 255 };
constexpr TGAColor blue = { 255, 128,  64, 255 };
constexpr TGAColor yellow = { 0, 200, 255, 255 };







int main(int argc, char** argv) {
    
    constexpr int width = 800;
    constexpr int height = 800;
    TGAImage framebuffer(width, height, TGAImage::RGB);


    model myModel("res/obj/diablo3_pose/diablo3_pose.obj", red, framebuffer);

    myModel.draw();

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}
