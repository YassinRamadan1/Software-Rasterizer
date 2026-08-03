#include "tgaimage.h"
#include "Mesh.h"
#include "utility.h"
#include "FrameBuffer.h"

#include "vertexProcessor.h"
#include "primitiveAssembler.h"
#include "rasterizer.h"
#include "camera.h"

constexpr int width = 800;
constexpr int height = 800;

std::string africanHeadLocation = RESOURCES_PATH + std::string("obj/african_head/african_head.obj"),
    africanHeadTexLocation = RESOURCES_PATH + std::string("obj/african_head/african_head_diffuse.tga"),
    africanHeadEyeInnerLocation = RESOURCES_PATH + std::string("obj/african_head/african_head_eye_inner.obj"),
    africanHeadEyeInnerTexLocation = RESOURCES_PATH + std::string("obj/african_head/african_head_eye_inner_diffuse.tga"),
    africanHeadEyeOuterLocation = RESOURCES_PATH + std::string("obj/african_head/african_head_eye_outer.obj"),
    africanHeadEyeOuterTexLocation = RESOURCES_PATH + std::string("obj/african_head/african_head_eye_outer_diffuse.tga"),
    diabloLocation = RESOURCES_PATH + std::string("obj/diablo3_pose/diablo3_pose.obj"),
    diabloTexLocation = RESOURCES_PATH + std::string("obj/diablo3_pose/diablo3_pose_diffuse.tga"),
    boggieHeadLocation = RESOURCES_PATH + std::string("obj/boggie/head.obj"),
    boggieHeadTexLocation = RESOURCES_PATH + std::string("obj/boggie/head_diffuse.tga"),
    boggieEyesLocation = RESOURCES_PATH + std::string("obj/boggie/eyes.obj"),
    boggieEyesTexLocation = RESOURCES_PATH + std::string("obj/boggie/eyes_diffuse.tga"),
    boggieBodyLocation = RESOURCES_PATH + std::string("obj/boggie/body.obj"),
    boggieBodyTexLocation = RESOURCES_PATH + std::string("obj/boggie/body_diffuse.tga");

FrameBuffer framebuffer(width, height);
Rasterizer r;

void diablo(FrameBuffer& framebuffer)
{
    Texture diabloTex(diabloTexLocation);
    diabloTex.generateMipmaps();
    Mesh diabloMesh(diabloLocation);

    Camera myCamera;
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));
    model = glm::scale(model, glm::vec3(5.));
    glm::mat4 view = myCamera.getViewMatrix();
    glm::mat4 proj = utility::perspectiveProjection(glm::radians(2 * myCamera.m_Zoom), width * 1.0f / height, 1.0f, 100.0f);
    glm::mat4 proj2 = utility::orthographicProjection(glm::radians(2 * myCamera.m_Zoom), width * 1.0f / height, 1.0f, 100.0f);
    glm::mat4 viewport = utility::viewport(0, 0, width, height);

    vertex_processor::processVertices(diabloMesh.vertices, proj * view * model);

    std::vector<glm::vec3> colors(diabloMesh.textureCoords.size(), glm::vec3(1.0, 0., 0.));
    //std::vector<Face> faces{ diabloMesh.faces[0]};
    primitive_assembler::processPrimitives(vertex_processor::transformedVertices, std::vector<glm::vec3>(), colors, diabloMesh.faces, viewport);

    r.setRenderMode(RenderMode::WIREFRAME);
    r.draw(primitive_assembler::triangles, framebuffer);
    framebuffer.storeColorBuffer(RESOURCES_PATH + std::string("diablo.tga"));
}

void africanHead(FrameBuffer& framebuffer)
{
    Texture africanHeadTex(africanHeadTexLocation);
    africanHeadTex.generateMipmaps();
    Mesh africanHeadMesh(africanHeadLocation);

    Camera myCamera;
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));

    glm::mat4 view = myCamera.getViewMatrix();
    glm::mat4 proj = utility::perspectiveProjection(glm::radians(2 * myCamera.m_Zoom), width * 1.0f / height, 1.0f, 100.0f);
    glm::mat4 proj2 = utility::orthographicProjection(glm::radians(2 * myCamera.m_Zoom), width * 1.0f / height, 1.0f, 100.0f);
    glm::mat4 viewport = utility::viewport(0, 0, width, height);

    vertex_processor::processVertices(africanHeadMesh.vertices, proj * view * model);

    primitive_assembler::processPrimitives(vertex_processor::transformedVertices, africanHeadMesh.textureCoords, std::vector<glm::vec3>(), africanHeadMesh.faces, viewport);

    r.setRenderMode(RenderMode::SOLID);
    r.draw(primitive_assembler::triangles, africanHeadTex, framebuffer);
    framebuffer.storeColorBuffer(RESOURCES_PATH + std::string("africanHeadWireFramed.tga"));
}

void triangle(FrameBuffer& framebuffer)
{
    std::vector<glm::vec4> vertices
    {
        glm::vec4(0.0, -3.0, -5.0, 1.0f),
        glm::vec4(3.0, 3.0,  -5.0, 1.0f),
        glm::vec4(-3.0, 3.0, -5.0, 1.0f)
    };

    std::vector<glm::vec3> colors
    {
        glm::vec3(1.0, 0.0, 0.0),
        glm::vec3(0.0, 1.0, 0.0),
        glm::vec3(0.0, 0.0, 1.0)
    };

    Face f;
    f.position = glm::vec3(0, 1, 2);
    f.color = f.position;
    std::vector<Face> faces
    {
        f
    };

    Camera myCamera;
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));

    glm::mat4 view = myCamera.getViewMatrix();
    glm::mat4 proj = utility::perspectiveProjection(glm::radians(2 * myCamera.m_Zoom), width * 1.0f / height, 1.0f, 100.0f);
    glm::mat4 proj2 = utility::orthographicProjection(glm::radians(2 * myCamera.m_Zoom), width * 1.0f / height, 1.0f, 100.0f);
    glm::mat4 viewport = utility::viewport(0, 0, width, height);

    vertex_processor::processVertices(vertices, proj * view * model);

    primitive_assembler::processPrimitives(vertex_processor::transformedVertices, std::vector<glm::vec3>(), colors, faces, viewport);

    r.setRenderMode(RenderMode::WIREFRAME);
    r.draw(primitive_assembler::triangles, framebuffer);
    framebuffer.storeColorBuffer(RESOURCES_PATH + std::string("triangle.tga"));
}

int main(int argc, char** argv)
{
   // boggieEyeTex(boggieEyesTexLocation), boggieBodyTex(boggieBodyTexLocation),
   //     boggieHeadTex(boggieHeadTexLocation),
    
   //     africanHeadEyeInnerTex(africanHeadEyeInnerTexLocation), africanHeadEyeOuterTex(africanHeadEyeOuterTexLocation);

    //
    //, boggieEye(boggieEyesLocation), boggieBody(boggieBodyLocation), boggieHead(boggieHeadLocation)
    //africanHeadEyeInner(africanHeadEyeInnerLocation), africanHeadEyeOuter(africanHeadEyeOuterLocation);

    
    //triangle(framebuffer);
    diablo(framebuffer);
    return 0;
}