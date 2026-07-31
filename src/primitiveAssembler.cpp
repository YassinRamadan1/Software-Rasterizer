#include "primitiveAssembler.h"

namespace primitive_assembler
{
    std::vector<Triangle> triangles;

    void processPrimitives(const std::vector<glm::vec4>& vertices, const std::vector<glm::vec3>& textureCoordinates, const std::vector<glm::vec3>& colors, const std::vector<Face>& faces, glm::mat4 viewport)
    {
        triangles.clear();
        triangles.reserve(faces.size());
        Triangle triangle;
        for (int i = 0; i < faces.size(); ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                triangle.position[j] = vertices[faces[i].position[j]];
    
                if (!textureCoordinates.empty())
                    triangle.textureCoord[j] = textureCoordinates[faces[i].textureCoord[j]];
                if (!colors.empty())
                    triangle.color[j] = colors[faces[i].color[j]];
            }
    
            float w;
            std::vector<Triangle> temp = clipper::clip(triangle);
            for (int j = 0; j < temp.size(); ++j)
            {
                for (auto& vertex : temp[j].position)
                {
                    w = -vertex.w;
    
                    vertex /= w;
                    vertex = viewport * vertex;
    
                    vertex.w = w;
                }
                triangles.emplace_back(temp[j]);
            }
        }
    }
};