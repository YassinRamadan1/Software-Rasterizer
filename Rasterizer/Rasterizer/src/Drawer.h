#pragma once

#include "utility.h"
class Drawer {

private:

    std::vector<Triangle>* m_Triangles;
    TextureProcessor* m_TextureProcessor;
    TGAImage* m_Framebuffer;
    float * m_DepthBuffer, m_Far, m_Near, m_Viewport_Near = 0, m_Viewport_Far = 1;
    RenderMode m_RenderMode;
    ProjectionMode m_ProjectionMode;
    AttributeMode m_AttributeMode;
public:

    void setTriangles(std::vector<Triangle>& triangles) {
        m_Triangles = &triangles;
    }

    void setTextureProcessor(TextureProcessor& textureProcessor) {

        m_TextureProcessor = &textureProcessor;
    }

    void setFramebuffer(TGAImage* framebuffer) {
        m_Framebuffer = framebuffer;
    }

    void setFar(float far) {
		m_Far = far;
	}

    void setNear(float near) {
        m_Near = near;
    }

    void setViewportNear(float viewportNear) {
		m_Viewport_Near = viewportNear;
	}

    void setViewportFar(float viewportFar) {
        m_Viewport_Far = viewportFar;
    }

    void setDepthBuffer(float* depthBuffer) {
        m_DepthBuffer = depthBuffer;
    }

    void setRenderMode(RenderMode renderMode) {
        m_RenderMode = renderMode;
    }

    void setProjectionMode(ProjectionMode projectionMode) {
        m_ProjectionMode = projectionMode;
    }

    void setAttributeMode(AttributeMode attributeMode) {
		m_AttributeMode = attributeMode;
	}

    void draw() {

        switch (m_RenderMode) {
        case WIREFRAME:
            //drawWireframe();
            break;
        case SOLID:
            switch (m_ProjectionMode) {
                case PERSPECTIVE:
                    switch (m_AttributeMode) {
                    case COLOR:
                        drawSolidPerspectiveColor();
                        break;
                    case TEXTURE:
                        drawSolidPerspectiveTexture();
                        break;
                    }
                    break;
                case ORTHOGRAPHIC:
                    switch (m_AttributeMode) {
                        case COLOR:
                            drawSolidOrthoColor();
                            break;
                        case TEXTURE:
                            drawSolidOrthoTexture();
                            break;
                    }
                    break;
            }
            break;
        }
    }

private:

    void drawSolidPerspectiveColor() {
        for (auto& triangle : (*m_Triangles)) {
			drawSolid1(triangle);
		}
    }

    void drawSolidOrthoColor() {
        for (auto& triangle : (*m_Triangles)) {
            drawSolid2(triangle);
        }
    }

    void drawSolidPerspectiveTexture() {
        for (auto& triangle : (*m_Triangles)) {
            drawSolid3(triangle);
        }
    }

    void drawSolidOrthoTexture() {
        for (auto& triangle : (*m_Triangles)) {
            drawSolid4(triangle);
        }
    }

    float cameraSpaceZ(float z) {
        float Zndc = 2 * (z - m_Viewport_Near) / (m_Viewport_Far - m_Viewport_Near) - 1;
		return (2 * m_Far * m_Near) / (m_Far + m_Near - Zndc * (m_Far - m_Near));
	}

    float viewportSpaceZ(float z) {
        return (m_Viewport_Near + (m_Viewport_Far - m_Viewport_Near) * (z - m_Near) / (m_Far - m_Near));
    }

    glm::dvec3 barycentric(double& area, glm::vec2& v0, glm::vec2& v1, glm::vec2& v2, glm::vec2 p) {
		double alpha = utility::cross(v1, v2, p) / area;
		double beta = utility::cross(v2, v0, p) / area;
		double gamma = utility::cross(v0, v1, p) / area;
		return glm::dvec3(alpha, beta, gamma);
	}

    float calcMipmapLevel(float u00, float v00, float u01, float v01, float u10, float v10, int width, int height) {

        float du01 = (u01 - u00) * width, dv01 = (v01 - v00) * height,
            du10 = (u10 - u00) * width, dv10 = (v10 - v00) * height,
            L = std::max(du01 * du01 + dv01 * dv01, du10 * du10 + dv10 * dv10);
        return std::log2(L) / 2.0f;
    }

    void drawSolid1(Triangle& triangle) {

        glm::vec2 v0 = glm::vec2(triangle.position[0]), v1 = glm::vec2(triangle.position[1]), v2 = glm::vec2(triangle.position[2]);

        // get the bounding box of the triangle and clip it to the framebuffer (clip against the left, right, bottom and top planes)

        int xmin = std::max(0.0f, std::round(std::min(v0.x, std::min(v1.x, v2.x)))),
            ymin = std::max(0.0f, std::round(std::min(v0.y, std::min(v1.y, v2.y)))),
            xmax = std::min(m_Framebuffer->width() - 1.0f, std::round(std::max(v0.x, std::max(v1.x, v2.x)))),
            ymax = std::min(m_Framebuffer->height() - 1.0f, std::round(std::max(v0.y, std::max(v1.y, v2.y))));

        double area = utility::cross(v0, v1, v2);

        
        float z0 = cameraSpaceZ(triangle.position[0].z),
            z1 = cameraSpaceZ(triangle.position[1].z),
            z2 = cameraSpaceZ(triangle.position[2].z);

        // it is an instruction to OpenMP to parallelize the loop since it is perfectly parallel
#pragma omp parallel for
        for (int y = ymin; y < ymax + 1; y++) {
            int index = y * m_Framebuffer->width();
            for (int x = xmin; x < xmax + 1; x++) {

                // get the barycentric coordinates alpha * P0 + beta * P1 + gamma * P2
                // watch out for orientation!
                glm::dvec3 current = barycentric(area, v0, v1, v2, glm::vec2(x, y));
                double alpha = current.x, beta = current.y, gamma = current.z;

                if (alpha < 0.0f || beta < 0.0f || gamma < 0.0f)
                    continue;

                // early depth testing
                float z_interp = alpha * 1 / z0 + beta * 1 / z1 + gamma * 1 / z2;
                z_interp = 1 / z_interp;

                if (z_interp < m_DepthBuffer[index + x]) {

                    float r_interp, g_interp, b_interp;
                    unsigned char r = 0, g = 0, b = 0;
                    r_interp = alpha * triangle.color[0].r / z0 + beta * triangle.color[1].r / z1 + gamma * triangle.color[2].r / z2;
                    r_interp *= z_interp;
                    g_interp = alpha * triangle.color[0].g / z0 + beta * triangle.color[1].g / z1 + gamma * triangle.color[2].g / z2;
                    g_interp *= z_interp;
                    b_interp = alpha * triangle.color[0].b / z0 + beta * triangle.color[1].b / z1 + gamma * triangle.color[2].b / z2;
                    b_interp *= z_interp;
                    
                    // clamp the color values
                    r = static_cast<unsigned char>(std::clamp(static_cast<int>(std::round(r_interp)), 0, 255));
                    g = static_cast<unsigned char>(std::clamp(static_cast<int>(std::round(g_interp)), 0, 255));
                    b = static_cast<unsigned char>(std::clamp(static_cast<int>(std::round(b_interp)), 0, 255));

                    // map back to viewport space
                    z_interp = viewportSpaceZ(z_interp);

                    // update the depth buffer
                    m_DepthBuffer[index + x] = z_interp;

                    // set the pixel color
                    m_Framebuffer->set(x, y, { b, g, r, 255 });
                }
            }
        }
    }

    void drawSolid2(Triangle& triangle) {

        glm::vec2 v0 = glm::vec2(triangle.position[0]), v1 = glm::vec2(triangle.position[1]), v2 = glm::vec2(triangle.position[2]);

        // get the bounding box of the triangle and clip it to the framebuffer (clip against the left, right, bottom and top planes)

        int xmin = std::max(0.0f, std::round(std::min(v0.x, std::min(v1.x, v2.x)))),
            ymin = std::max(0.0f, std::round(std::min(v0.y, std::min(v1.y, v2.y)))),
            xmax = std::min(m_Framebuffer->width() - 1.0f, std::round(std::max(v0.x, std::max(v1.x, v2.x)))),
            ymax = std::min(m_Framebuffer->height() - 1.0f, std::round(std::max(v0.y, std::max(v1.y, v2.y))));

        double area = utility::cross(v0, v1, v2);


        float z0 = triangle.position[0].z, z1 = triangle.position[1].z, z2 = triangle.position[2].z;

        // it is an instruction to OpenMP to parallelize the loop since it is perfectly parallel
#pragma omp parallel for
        for (int y = ymin; y < ymax + 1; y++) {
            int index = y * m_Framebuffer->width();
            for (int x = xmin; x < xmax + 1; x++) {

                // get the barycentric coordinates alpha * P0 + beta * P1 + gamma * P2
                // watch out for orientation!
                glm::dvec3 current = barycentric(area, v0, v1, v2, glm::vec2(x, y));
                double alpha = current.x, beta = current.y, gamma = current.z;

                if (alpha < 0.0f || beta < 0.0f || gamma < 0.0f)
                    continue;

                // early depth testing
                float z_interp = alpha * z0 + beta * z1 + gamma * z2;

                if (z_interp < m_DepthBuffer[index + x]) {

                    // update the depth buffer
                    m_DepthBuffer[index + x] = z_interp;

                    float r_interp, g_interp, b_interp;
                    unsigned char r = 0, g = 0, b = 0;
                    r_interp = alpha * triangle.color[0].r + beta * triangle.color[1].r + gamma * triangle.color[2].r;
                    g_interp = alpha * triangle.color[0].g + beta * triangle.color[1].g + gamma * triangle.color[2].g;
                    b_interp = alpha * triangle.color[0].b + beta * triangle.color[1].b + gamma * triangle.color[2].b;

                    // clamp the color values
                    r = static_cast<unsigned char>(std::clamp(static_cast<int>(std::round(r_interp)), 0, 255));
                    g = static_cast<unsigned char>(std::clamp(static_cast<int>(std::round(g_interp)), 0, 255));
                    b = static_cast<unsigned char>(std::clamp(static_cast<int>(std::round(b_interp)), 0, 255));

                    // set the pixel color
                    m_Framebuffer->set(x, y, { b, g, r, 255 });
                }
            }
        }
    }

    void drawSolid3(Triangle& triangle) {

        glm::vec2 v0 = glm::vec2(triangle.position[0]), v1 = glm::vec2(triangle.position[1]), v2 = glm::vec2(triangle.position[2]);

        // get the bounding box of the triangle and clip it to the framebuffer (clip against the left, right, bottom and top planes)

        int xmin = std::max(0.0f, std::round(std::min(v0.x, std::min(v1.x, v2.x)))),
            ymin = std::max(0.0f, std::round(std::min(v0.y, std::min(v1.y, v2.y)))),
            xmax = std::min(m_Framebuffer->width() - 1.0f, std::round(std::max(v0.x, std::max(v1.x, v2.x)))),
            ymax = std::min(m_Framebuffer->height() - 1.0f, std::round(std::max(v0.y, std::max(v1.y, v2.y))));

        double area = utility::cross(v0, v1, v2);
        float mipmapLevel = 0;
        glm::vec3 depth(1 / cameraSpaceZ(triangle.position[0].z), 1 / cameraSpaceZ(triangle.position[1].z), 1 / cameraSpaceZ(triangle.position[2].z)),
            textureCoordU(triangle.textureCoord[0].s / cameraSpaceZ(triangle.position[0].z), triangle.textureCoord[1].s / cameraSpaceZ(triangle.position[1].z), triangle.textureCoord[2].s / cameraSpaceZ(triangle.position[2].z)),
            textureCoordV(triangle.textureCoord[0].t / cameraSpaceZ(triangle.position[0].z), triangle.textureCoord[1].t / cameraSpaceZ(triangle.position[1].z), triangle.textureCoord[2].t / cameraSpaceZ(triangle.position[2].z));

        for (int y = ymin; y < ymax + 1; y++) {
            int index = y * m_Framebuffer->width();
            for (int x = xmin; x < xmax + 1; x++) {

                // get the barycentric coordinates alpha * P0 + beta * P1 + gamma * P2
                // watch out for orientation!
                glm::vec3 current = barycentric(area, v0, v1, v2, glm::vec2(x, y));
                double alpha = current.x, beta = current.y, gamma = current.z;
                

                if (alpha < 0.0f || beta < 0.0f || gamma < 0.0f)
                    continue;

                glm::vec3 right = barycentric(area, v0, v1, v2, glm::vec2(x + 1, y));
                glm::vec3 up = barycentric(area, v0, v1, v2, glm::vec2(x, y + 1));
                
                // early depth testing
                float z_interp = glm::dot(current, depth);
                z_interp = 1 / z_interp;

                if (z_interp < m_DepthBuffer[index + x]) {

                    float z_interp_right = glm::dot(right, depth),
                        z_interp_up = glm::dot(up, depth);
                    z_interp_right = 1 / z_interp_right, z_interp_up = 1 / z_interp_up;

                    float u_interp = z_interp * glm::dot(current, textureCoordU), v_interp = z_interp * glm::dot(current, textureCoordV),
                        u_interp_right = z_interp_right * glm::dot(right, textureCoordU), v_interp_right = z_interp_right * glm::dot(right, textureCoordV),
                        u_interp_up = z_interp_up * glm::dot(up, textureCoordU), v_interp_up = z_interp_up * glm::dot(up, textureCoordV);
                    
                    if(x != xmax && y != ymax)
                        mipmapLevel = calcMipmapLevel(u_interp, v_interp, u_interp_right, v_interp_right, u_interp_up, v_interp_up, m_TextureProcessor->getTextureWidth(0), m_TextureProcessor->getTextureHeight(0));

                    // set the pixel color
                    m_Framebuffer->set(x, y, m_TextureProcessor->getTexel(u_interp, v_interp, mipmapLevel));

                    // map back to viewport space
                    z_interp = viewportSpaceZ(z_interp);

                    // update the depth buffer
                    m_DepthBuffer[index + x] = z_interp;

                }
            }
        }
    }

    void drawSolid4(Triangle& triangle) {

        glm::vec2 v0 = glm::vec2(triangle.position[0]), v1 = glm::vec2(triangle.position[1]), v2 = glm::vec2(triangle.position[2]);

        // get the bounding box of the triangle and clip it to the framebuffer (clip against the left, right, bottom and top planes)

        int xmin = std::max(0.0f, std::round(std::min(v0.x, std::min(v1.x, v2.x)))),
            ymin = std::max(0.0f, std::round(std::min(v0.y, std::min(v1.y, v2.y)))),
            xmax = std::min(m_Framebuffer->width() - 1.0f, std::round(std::max(v0.x, std::max(v1.x, v2.x)))),
            ymax = std::min(m_Framebuffer->height() - 1.0f, std::round(std::max(v0.y, std::max(v1.y, v2.y))));

        double area = utility::cross(v0, v1, v2);

        float z0 = triangle.position[0].z, z1 = triangle.position[1].z, z2 = triangle.position[2].z;

        // it is an instruction to OpenMP to parallelize the loop since it is perfectly parallel
#pragma omp parallel for
        for (int y = ymin; y < ymax + 1; y++) {
            int index = y * m_Framebuffer->width();
            for (int x = xmin; x < xmax + 1; x++) {

                // get the barycentric coordinates alpha * P0 + beta * P1 + gamma * P2
                // watch out for orientation!
                glm::dvec3 current = barycentric(area, v0, v1, v2, glm::vec2(x, y));
                double alpha = current.x, beta = current.y, gamma = current.z;

                if (alpha < 0.0f || beta < 0.0f || gamma < 0.0f)
                    continue;

                // early depth testing
                float z_interp = alpha * z0 + beta * z1 + gamma * z2;

                if (z_interp < m_DepthBuffer[index + x]) {

                    // update the depth buffer
                    m_DepthBuffer[index + x] = z_interp;

                    float u_interp, v_interp;
                    u_interp = alpha * triangle.textureCoord[0].s + beta * triangle.textureCoord[1].s + gamma * triangle.textureCoord[2].s;
                    v_interp = alpha * triangle.textureCoord[0].t + beta * triangle.textureCoord[1].t + gamma * triangle.textureCoord[2].t;
                    
                    // set the pixel color
                    m_Framebuffer->set(x, y, m_TextureProcessor->getTexel(u_interp, v_interp, 0));
                }
            }
        }
    }

};