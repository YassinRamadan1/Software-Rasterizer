#include "tgaimage.h"
#include "Model.h"

constexpr TGAColor white = { 255, 255, 255, 255 }; // attention, BGRA order
constexpr TGAColor green = { 0, 255,   0, 255 };
constexpr TGAColor red = { 0,   0, 255, 255 };
constexpr TGAColor blue = { 255, 128,  64, 255 };
constexpr TGAColor yellow = { 0, 200, 255, 255 };

int main(int argc, char** argv) {
    
    constexpr int width = 800;
    constexpr int height = 800;
    TGAImage framebuffer(width, height, TGAImage::RGB);
    TGAImage texture;
    texture.read_tga_file("res/obj/african_head/african_head_diffuse.tga");
    texture.flip_vertically();
    float* zBuffer = new float [800 * 800];

    for(int i = 0; i < width * height; i++)
		zBuffer[i] = std::numeric_limits<float>::max();
    srand(time(0));

    glm::vec3 v1 = glm::vec3(-0.5f, 0.5f, -10.0f),
        v2 = glm::vec3(0.5f, 0.5f, -2.0f),
        v3 = glm::vec3(0.0f, -0.5f, -2.0f);
    utility::Camera myCamera;
    //myCamera.processMouseMovement(20.0f, 0.0f);
    
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f)); //glm::mat4(1.0f);// glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    glm::mat4 view = myCamera.getViewMatrix();
    glm::mat4 proj = utility::perspectiveProjection(glm::radians(myCamera.m_Zoom), 1.0f, 1.0f, 100.0f);
    glm::mat4 proj2 = utility::orthographicProjection(glm::radians(2 * myCamera.m_Zoom), 1.0f, 1.0f, 100.0f);
    glm::mat4  viewport = utility::viewport(0, 0, width, height);

    utility::Transform transform(model, view, proj2, viewport);
    transform.near = 1.0f, transform.far = 100.0f;
/*    utility::Triangle triangle(v1, v2, v3, glm::vec3(255.0f, 0.0f, 0.0f), glm::vec3(0.0f, 255.0f, 0.0f), glm::vec3(0.0f, 0.0f, 255.0f));
    rasterize(triangle, transform, texture, zBuffer, framebuffer);
*/
    Model myModel("res/obj/african_head/african_head.obj", framebuffer);
    myModel.draw(RenderingMode::SOLID, transform, zBuffer, texture);
    delete zBuffer;
    
    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}