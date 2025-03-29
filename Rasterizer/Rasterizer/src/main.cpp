#include "tgaimage.h"
#include "Mesh.h"
#include "Rasterizer.h"
#include "utility.h"
#include "FrameBuffer.h"

constexpr int width = 800;
constexpr int height = 800;

const char* African_Head_Location = "res/obj/african_head/african_head.obj",
* African_Head_Texture_Location = "res/obj/african_head/african_head_diffuse.tga",

* African_Head_Eye_Inner_Location = "res/obj/african_head/african_head_eye_inner.obj",
* African_Head_Eye_Inner_Texture_Location = "res/obj/african_head/african_head_eye_inner_diffuse.tga",

* African_Head_Eye_Outer_Location = "res/obj/african_head/african_head_eye_outer.obj",
* African_Head_Eye_Outer_Texture_Location = "res/obj/african_head/african_head_eye_outer_diffuse.tga",

* Diablo_Location = "res/obj/diablo3_pose/diablo3_pose.obj",
* Diablo_Texture_Location = "res/obj/diablo3_pose/diablo3_pose_diffuse.tga",

* Boggie_Head_Location = "res/obj/boggie/head.obj",
* Boggie_Head_Texture_Location = "res/obj/boggie/head_diffuse.tga",

* Boggie_Eyes_Location = "res/obj/boggie/eyes.obj",
* Boggie_Eyes_Texture_Location = "res/obj/boggie/eyes_diffuse.tga",

* Boggie_Body_Location = "res/obj/boggie/body.obj",
* Boggie_Body_Texture_Location = "res/obj/boggie/body_diffuse.tga";

int main(int argc, char** argv) {

    FrameBuffer framebuffer(width, height, TGAImage::RGB);

    TGAImage texture1, texture2, texture3;
    texture1.read_tga_file(Boggie_Head_Texture_Location);
    texture1.flip_vertically();
    texture2.read_tga_file(Boggie_Eyes_Texture_Location);
    texture2.flip_vertically();
   texture3.read_tga_file(Boggie_Body_Texture_Location);
   texture3.flip_vertically();

    Mesh Model1(Boggie_Head_Location), Model2(Boggie_Eyes_Location), Model3(Boggie_Body_Location);

    utility::Camera myCamera;
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));

    glm::mat4 view = myCamera.getViewMatrix();
    glm::mat4 proj = utility::perspectiveProjection(glm::radians(myCamera.m_Zoom), width * 1.0f / height, 1.0f, 100.0f);
    glm::mat4 proj2 = utility::orthographicProjection(glm::radians(2 * myCamera.m_Zoom), width * 1.0f / height, 1.0f, 100.0f);

    Rasterizer myRasterizer(Model1.m_Vertices, Model1.m_Faces, model, view, proj, framebuffer, texture1, RenderMode::SOLID, ProjectionMode::PERSPECTIVE);
    myRasterizer.setFilterMode(FilterMode::BILINEAR);
    myRasterizer.setWrapModeU(WrapMode::CLAMP);
    myRasterizer.setWrapModeV(WrapMode::CLAMP);
    myRasterizer.setTextureCoords(Model1.m_Textures);
    myRasterizer.process();
    myRasterizer.draw();

    myRasterizer.setVertices(Model2.m_Vertices);
    myRasterizer.setFaces(Model2.m_Faces);
    myRasterizer.setTexture(texture2);
    myRasterizer.setTextureCoords(Model2.m_Textures);
    myRasterizer.process();
    myRasterizer.draw();

    myRasterizer.setVertices(Model3.m_Vertices);
    myRasterizer.setFaces(Model3.m_Faces);
    myRasterizer.setTexture(texture3);
    myRasterizer.setTextureCoords(Model3.m_Textures);
    myRasterizer.process();
    myRasterizer.draw();

    framebuffer.write_Color_as_tga_file("Export/Image.tga");

    return 0;
}