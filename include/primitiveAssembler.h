#pragma once
#include "utility.h"
#include "clipper.h"

namespace primitive_assembler
{
    extern std::vector<Triangle> triangles;

    void processPrimitives(const std::vector<glm::vec4>& vertices, const std::vector<glm::vec3>& textureCoordinates, const std::vector<glm::vec3>& colors, const std::vector<Face>& faces, glm::mat4 viewport);
};