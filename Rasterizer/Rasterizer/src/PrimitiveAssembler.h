#pragma once

#include "utility.h"
#include "Color.h"

class PrimitiveAssembler {

private:

    std::vector<Triangle> m_Triangles;

    std::vector<glm::vec4>* m_Vertices;
    std::vector<glm::vec3>* m_TextureCoordinates;
    std::vector<Color>* m_Colors;
    std::vector<Face>* m_Faces;
    glm::mat4* m_Viewport;

public:

    PrimitiveAssembler();

    PrimitiveAssembler(std::vector<glm::vec4>& vertices, std::vector<glm::vec3>& textures, std::vector<Face>& faces);

    PrimitiveAssembler(std::vector<glm::vec4>& vertices, std::vector<glm::vec3>& textures,
        std::vector<Color>& colors, std::vector<Face>& faces);

private:
    std::vector<Triangle> clipFarPlane(Triangle& triangle);

    std::vector<Triangle> clipNearPlane(Triangle& triangle);

    std::vector<Triangle> clip(Triangle& triangle);

public:

    void setVertices(std::vector<glm::vec4>& vertices);

    void setTextureCoordinates(std::vector<glm::vec3>& textures);
    
    void setColors(std::vector<Color>& colors);
    
    void setFaces(std::vector<Face>& faces);

    void setViewport(glm::mat4& viewport);

    void processPrimitives();

    std::vector<Triangle>& getTriangles();
};