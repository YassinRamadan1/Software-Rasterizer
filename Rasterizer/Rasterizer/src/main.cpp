#include "tgaimage.h"
#include "model.h"

constexpr TGAColor white = { 255, 255, 255, 255 }; // attention, BGRA order
constexpr TGAColor green = { 0, 255,   0, 255 };
constexpr TGAColor red = { 0,   0, 255, 255 };
constexpr TGAColor blue = { 255, 128,  64, 255 };
constexpr TGAColor yellow = { 0, 200, 255, 255 };







int main(int argc, char** argv) {
    
    constexpr int width = 200;
    constexpr int height = 200;
    TGAImage framebuffer(width, height, TGAImage::RGB);


   // model myModel("res/obj/diablo3_pose/diablo3_pose.obj", red, framebuffer);

   // myModel.draw();
    drawTriangleFilled(7, 45, 35, 100, 45, 60, framebuffer, red);
    drawTriangleWireFrame(7, 45, 35, 100, 45, 60, framebuffer, green);

    drawTriangleFilled(120, 35, 90, 5, 45, 110, framebuffer, white);
    drawTriangleFilled(115, 83, 80, 90, 85, 120, framebuffer, blue);

    drawTriangleWireFrame(120, 35, 90, 5, 45, 110, framebuffer, green);
    drawTriangleWireFrame(115, 83, 80, 90, 85, 120, framebuffer, green);
    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}
