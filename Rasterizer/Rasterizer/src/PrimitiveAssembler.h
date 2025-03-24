#pragma once

#include "utility.h"

class PrimitiveAssembler {

private:

    std::vector<Triangle> m_Triangles;

    std::vector<glm::vec4>* m_Vertices;
    std::vector<glm::vec3>* m_TextureCoordinates;
    std::vector<glm::vec4>* m_Colors;
    std::vector<Face>* m_Faces;
    glm::mat4* m_Viewport;

    std::vector<Triangle> clipFarPlane(Triangle& triangle) {

        std::vector<Triangle> triangles;
        bool state[3] = { false, false, false };
        // relax the comparison (consider z value equals the near plane)
        state[0] = (triangle.position[0].z > triangle.position[0].w),
            state[1] = (triangle.position[1].z > triangle.position[1].w),
            state[2] = (triangle.position[2].z > triangle.position[2].w);
        int sum = state[0] + state[1] + state[2];

        if (sum == 3) { // the whole triangle is rejected
            return triangles;
        }

        if (!sum) {   // the whole triangle is accepted

            triangles.push_back(triangle);
            return triangles;
        }

        if (sum == 1) { // one vertex is clipped

            // make state[0] always true (has to be clipped)
            if (state[1]) {
                std::swap(triangle.position[0], triangle.position[1]);
                std::swap(triangle.textureCoord[0], triangle.textureCoord[1]);
                std::swap(triangle.color[0], triangle.color[1]);
                std::swap(state[0], state[1]);
            }
            else if (state[2]) {
                std::swap(triangle.position[0], triangle.position[2]);
                std::swap(triangle.textureCoord[0], triangle.textureCoord[2]);
                std::swap(triangle.color[0], triangle.color[2]);
                std::swap(state[0], state[2]);
            }

            // find intersection 1
            float t1 = (-triangle.position[0].z + triangle.position[0].w) / (triangle.position[1].z - triangle.position[1].w - triangle.position[0].z + triangle.position[0].w);
            glm::vec4 position1 = triangle.position[0] + t1 * (triangle.position[1] - triangle.position[0]);
            glm::vec3 textureCoord1 = triangle.textureCoord[0] + t1 * (triangle.textureCoord[1] - triangle.textureCoord[0]);
            glm::vec4 color1 = triangle.color[0] + t1 * (triangle.color[1] - triangle.color[0]);
            Triangle triangle1 = triangle;
            triangle1.position[0] = position1;
            triangle1.textureCoord[0] = textureCoord1;
            triangle1.color[0] = color1;
            triangles.push_back(triangle1);

            // find intersection 2
            float t2 = (-triangle.position[0].z + triangle.position[0].w) / (triangle.position[2].z - triangle.position[2].w - triangle.position[0].z + triangle.position[0].w);
            glm::vec4 position2 = triangle.position[0] + t2 * (triangle.position[2] - triangle.position[0]);
            glm::vec3 textureCoord2 = triangle.textureCoord[0] + t2 * (triangle.textureCoord[2] - triangle.textureCoord[0]);
            glm::vec4 color2 = triangle.color[0] + t2 * (triangle.color[2] - triangle.color[0]);
            Triangle triangle2 = triangle1;
            triangle2.position[1] = position2;
            triangle2.textureCoord[1] = textureCoord2;
            triangle2.color[1] = color2;
            triangles.push_back(triangle2);

            return triangles;
        }
        // two vertices are clipped

        // make state[0] always false (the other two vertices have to be clipped)
        if (state[0]) {

            std::swap(triangle.position[0], triangle.position[1]);
            std::swap(triangle.textureCoord[0], triangle.textureCoord[1]);
            std::swap(triangle.color[0], triangle.color[1]);
            std::swap(state[0], state[1]);
        }
        if (state[0]) {

            std::swap(triangle.position[0], triangle.position[2]);
            std::swap(triangle.textureCoord[0], triangle.textureCoord[2]);
            std::swap(triangle.color[0], triangle.color[2]);
            std::swap(state[0], state[2]);
        }

        // find intersection 1
        float t1 = (-triangle.position[1].z + triangle.position[1].w) / (triangle.position[0].z - triangle.position[0].w - triangle.position[1].z + triangle.position[1].w);
        glm::vec4 position1 = triangle.position[1] + t1 * (triangle.position[0] - triangle.position[1]);
        glm::vec3 textureCoord1 = triangle.textureCoord[1] + t1 * (triangle.textureCoord[0] - triangle.textureCoord[1]);
        glm::vec4 color1 = triangle.color[1] + t1 * (triangle.color[0] - triangle.color[1]);
        Triangle triangle1 = triangle;
        triangle1.position[1] = position1;
        triangle1.textureCoord[1] = textureCoord1;
        triangle1.color[1] = color1;

        // find intersection 2
        float t2 = (-triangle.position[2].z + triangle.position[2].w) / (triangle.position[0].z - triangle.position[0].w - triangle.position[2].z + triangle.position[2].w);
        glm::vec4 position2 = triangle.position[2] + t2 * (triangle.position[0] - triangle.position[2]);
        glm::vec3 textureCoord2 = triangle.textureCoord[2] + t2 * (triangle.textureCoord[0] - triangle.textureCoord[2]);
        glm::vec4 color2 = triangle.color[2] + t2 * (triangle.color[0] - triangle.color[2]);
        triangle1.position[2] = position2;
        triangle1.textureCoord[2] = textureCoord2;
        triangle1.color[2] = color2;

        triangles.push_back(triangle1);

        return triangles;
    }

    std::vector<Triangle> clipNearPlane(Triangle& triangle) {

        std::vector<Triangle> triangles;
        bool state[3] = { false, false, false };
        // relax the comparison (consider z value equals the near plane)
        state[0] = (triangle.position[0].z < -triangle.position[0].w);
        state[1] = (triangle.position[1].z < -triangle.position[1].w);
        state[2] = (triangle.position[2].z < -triangle.position[2].w);

        int sum = state[0] + state[1] + state[2];
        if (sum == 3) { // the whole triangle is rejected

            return triangles;
        }

        if (!sum) {   // the whole triangle is accepted

            triangles.push_back(triangle);
            return triangles;
        }

        if (sum == 1) { // one vertex is clipped

            // make state[0] always true (P0 has to be clipped)
            if (state[1]) {
                std::swap(triangle.position[0], triangle.position[1]);
                std::swap(triangle.textureCoord[0], triangle.textureCoord[1]);
                std::swap(triangle.color[0], triangle.color[1]);
                std::swap(state[0], state[1]);
            }
            else if (state[2]) {
                std::swap(triangle.position[0], triangle.position[2]);
                std::swap(triangle.textureCoord[0], triangle.textureCoord[2]);
                std::swap(triangle.color[0], triangle.color[2]);
                std::swap(state[0], state[2]);
            }

            // find intersection 1 between P0 and P1
            float t1 = -(triangle.position[0].z + triangle.position[0].w) / (triangle.position[1].z + triangle.position[1].w - triangle.position[0].z - triangle.position[0].w);
            glm::vec4 position1 = triangle.position[0] + t1 * (triangle.position[1] - triangle.position[0]);
            glm::vec3 textureCoord1 = triangle.textureCoord[0] + t1 * (triangle.textureCoord[1] - triangle.textureCoord[0]);
            glm::vec4 color1 = triangle.color[0] + t1 * (triangle.color[1] - triangle.color[0]);
            Triangle triangle1 = triangle; // triangle1 is P0, P1, P2
            triangle1.position[0] = position1;
            triangle1.textureCoord[0] = textureCoord1;
            triangle1.color[0] = color1;
            triangles.push_back(triangle1); // triangle1 is newP0 along edge(P0, P1), P1, P2

            // find intersection 2 between P0 and P2
            float t2 = -(triangle.position[0].z + triangle.position[0].w) / (triangle.position[2].z + triangle.position[2].w - triangle.position[0].z - triangle.position[0].w);
            glm::vec4 position2 = triangle.position[0] + t2 * (triangle.position[2] - triangle.position[0]);
            glm::vec3 textureCoord2 = triangle.textureCoord[0] + t2 * (triangle.textureCoord[2] - triangle.textureCoord[0]);
            glm::vec4 color2 = triangle.color[0] + t2 * (triangle.color[2] - triangle.color[0]);
            Triangle triangle2 = triangle1; // triangle2 is newP0 along edge(P0, P1), P1, P2
            triangle2.position[1] = position2;
            triangle2.textureCoord[1] = textureCoord2;
            triangle2.color[1] = color2;
            triangles.push_back(triangle2); // triangle2 is newP0 along edge(P0, P1), newP1 along edge(P0, P2), P2

            return triangles;
        }
        // two vertices are clipped

        // make state[0] always false (the other two vertices have to be clipped)
        if (state[0]) {

            std::swap(triangle.position[0], triangle.position[1]);
            std::swap(triangle.textureCoord[0], triangle.textureCoord[1]);
            std::swap(triangle.color[0], triangle.color[1]);
            std::swap(state[0], state[1]);
        }
        if (state[0]) {

            std::swap(triangle.position[0], triangle.position[2]);
            std::swap(triangle.textureCoord[0], triangle.textureCoord[2]);
            std::swap(triangle.color[0], triangle.color[2]);
            std::swap(state[0], state[2]);
        }

        // find intersection 1 between P1 and P0
        float t1 = -(triangle.position[1].z + triangle.position[1].w) / (triangle.position[0].z + triangle.position[0].w - triangle.position[1].z - triangle.position[1].w);
        glm::vec4 position1 = triangle.position[1] + t1 * (triangle.position[0] - triangle.position[1]);
        glm::vec3 textureCoord1 = triangle.textureCoord[1] + t1 * (triangle.textureCoord[0] - triangle.textureCoord[1]);
        glm::vec4 color1 = triangle.color[1] + t1 * (triangle.color[0] - triangle.color[1]);
        Triangle triangle1 = triangle; // triangle1 P0, P1, P2
        triangle1.position[1] = position1;
        triangle1.textureCoord[1] = textureCoord1;
        triangle1.color[1] = color1; // triangle1 P0, newP1 along edge(P1, P0), P2

        // find intersection 2 between P2 and P0
        float t2 = -(triangle.position[2].z + triangle.position[2].w) / (triangle.position[0].z + triangle.position[0].w - triangle.position[2].z - triangle.position[2].w);
        glm::vec4 position2 = triangle.position[2] + t2 * (triangle.position[0] - triangle.position[2]);
        glm::vec3 textureCoord2 = triangle.textureCoord[2] + t2 * (triangle.textureCoord[0] - triangle.textureCoord[2]);
        glm::vec4 color2 = triangle.color[2] + t2 * (triangle.color[0] - triangle.color[2]);
        triangle1.position[2] = position2;
        triangle1.textureCoord[2] = textureCoord2;
        triangle1.color[2] = color2; // triangle1 P0, newP1 along edge(P1, P0), newP2 along edge(P2, P0)

        triangles.push_back(triangle1);

        return triangles;
    }

    std::vector<Triangle> clip(Triangle& triangle) {

        std::vector<Triangle> temp = clipNearPlane(triangle), temp2, triangles;
        for (int i = 0; i < temp.size(); i++) {
            temp2 = clipFarPlane(temp[i]);
            for (int j = 0; j < temp2.size(); j++)
                triangles.push_back(temp2[j]);
        }
        return triangles;
    }
public:

	

	PrimitiveAssembler() : m_Vertices(nullptr), m_TextureCoordinates(nullptr), m_Colors(nullptr), m_Faces(nullptr) {}

    PrimitiveAssembler(std::vector<glm::vec4>& vertices, std::vector<glm::vec3>& textures, std::vector<Face>& faces)
        : m_Vertices(&vertices), m_TextureCoordinates(&textures), m_Faces(&faces) {}

    PrimitiveAssembler(std::vector<glm::vec4>& vertices, std::vector<glm::vec3>& textures,
        std::vector<glm::vec4>& colors, std::vector<Face>& faces)
        : m_Vertices(&vertices), m_TextureCoordinates(&textures), m_Colors(&colors), m_Faces(&faces) {}

    void setVertices(std::vector<glm::vec4>& vertices) { m_Vertices = &vertices; }

    void setTextureCoordinates(std::vector<glm::vec3>& textures) { m_TextureCoordinates = &textures; }
    
    void setColors(std::vector<glm::vec4>& colors) { m_Colors = &colors; }
    
    void setFaces(std::vector<Face>& faces) { m_Faces = &faces; }

    void setViewport(glm::mat4& viewport) { m_Viewport = &viewport; }

    void processPrimitives() {

        Triangle triangle;
		for (int i = 0; i < m_Faces->size(); i++) {

			for (int j = 0; j < 3; j++) {
				triangle.position[j] = (*m_Vertices)[(*m_Faces)[i].position[j]];
                
                if(m_TextureCoordinates)
				    triangle.textureCoord[j] = (*m_TextureCoordinates)[(*m_Faces)[i].texture[j]];
				if (m_Colors)
					triangle.color[j] = (*m_Colors)[(*m_Faces)[i].color[j]];
			}

			std::vector<Triangle> temp = clip(triangle);
            for (int j = 0; j < temp.size(); j++) {
                for (auto& vertex : temp[j].position) {
                    vertex /= vertex.w;
                    vertex = (*m_Viewport) * vertex;
                }
                m_Triangles.push_back(temp[j]);
            }
		}
	}

    std::vector<Triangle>& getTriangles() { return m_Triangles; }
};