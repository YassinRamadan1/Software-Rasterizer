#include "tgaimage.h"
#include "Model.h"
#include "Rasterizer.h"

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
    
    const char* texLocation1 = "res/obj/african_head/african_head_diffuse.tga",
        * texLocation2 = "res/obj/diablo3_pose/diablo3_pose_diffuse.tga",
    *texLocation3 = "res/checkerboard.tga";
    texture.read_tga_file(texLocation1);
    texture.flip_vertically();
    const char* location1 = "res/obj/african_head/african_head.obj",
        *location2 = "res/obj/diablo3_pose/diablo3_pose.obj";
    Model myModel(location1);

    float* zBuffer = new float [800 * 800];
    for(int i = 0; i < width * height; i++)
		zBuffer[i] = std::numeric_limits<float>::max();

    utility::Camera myCamera;

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));
    //model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 view = myCamera.getViewMatrix();

    glm::mat4 proj = utility::perspectiveProjection(glm::radians(myCamera.m_Zoom), 1.0f, 1.0f, 100.0f);
    glm::mat4 proj2 = utility::orthographicProjection(glm::radians(2 * myCamera.m_Zoom), 1.0f, 1.0f, 100.0f);

    glm::mat4  viewport = utility::viewport(0, 0, width, height);

    std::vector<glm::vec4> vertices(myModel.m_Vertices.size());
    for (int i = 0; i < vertices.size(); i++)
        vertices[i] = glm::vec4(myModel.m_Vertices[i], 1.0f);

    /*std::vector<glm::vec4> vertices{

        glm::vec4(1.0f, 1.0f, -1.0f, 1.0f),
        glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f),
        glm::vec4(1.0f, -1.0f, -1.0f, 1.0f),
        glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f)
    };*/
    
    std::vector<glm::vec3> textureCoords{

        glm::vec3(1.0, 1.0f, 0.0f),
        glm::vec3(0.0, 1.0f, 0.0f),
        glm::vec3(1.0, 0.0f, 0.0f),
        glm::vec3(0.0, 0.0f, 0.0f)
    };

    std::vector<Face> faces{

        Face(0, 1, 2, 0, 1, 2),
        Face(1, 3, 2, 1, 3, 2)
    };

    Rasterizer myRasterizer(vertices, myModel.m_Faces, model, view, proj2, viewport, framebuffer, texture, zBuffer, SOLID, ORTHOGRAPHIC);
    myRasterizer.setFilterMode(BILINEAR);
    myRasterizer.setWrapModeU(CLAMP);
    myRasterizer.setWrapModeV(CLAMP);
    myRasterizer.setTextureCoords(myModel.m_Textures);
    myRasterizer.process();
    myRasterizer.draw();
    framebuffer.write_tga_file("framebuffer.tga");

    delete zBuffer;

    return 0;
}