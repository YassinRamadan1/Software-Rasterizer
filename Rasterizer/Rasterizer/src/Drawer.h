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
        case RenderMode::WIREFRAME:
            //drawWireframe();
            break;
        case RenderMode::SOLID:
            switch (m_ProjectionMode) {
            case ProjectionMode::PERSPECTIVE:
                    switch (m_AttributeMode) {
                    case AttributeMode::COLOR:
                        drawSolidPerspectiveColor();
                        break;
                    case AttributeMode::TEXTURE:
                        drawSolidPerspectiveTexture();
                        break;
                    }
                    break;
            case ProjectionMode::ORTHOGRAPHIC:
                    switch (m_AttributeMode) {
                    case AttributeMode::COLOR:
                            drawSolidOrthoColor();
                            break;
                    case AttributeMode::TEXTURE:
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

        utility::Color c;

        glm::vec3 depth(1 / triangle.position[0].w, 1 / triangle.position[1].w, 1 / triangle.position[2].w),
            colorRed(triangle.color[0].r / triangle.position[0].w,
                triangle.color[1].r / triangle.position[1].w, triangle.color[2].r / triangle.position[2].w),
            colorGreen(triangle.color[0].g / triangle.position[0].w,
                triangle.color[1].g / triangle.position[1].w, triangle.color[2].g / triangle.position[2].w),
            colorBlue(triangle.color[0].b / triangle.position[0].w,
                triangle.color[1].b / triangle.position[1].w, triangle.color[2].b / triangle.position[2].w);

        for (int y = ymin; y < ymax + 1; y++) {
            int index = y * m_Framebuffer->width();
            for (int x = xmin; x < xmax + 1; x++) {

                // get the barycentric coordinates alpha * P0 + beta * P1 + gamma * P2
                // watch out for orientation!
                glm::vec3 current = barycentric(area, v0, v1, v2, glm::vec2(x, y));

                if (current.x < 0.0f || current.y < 0.0f || current.z < 0.0f)
                    continue;

                // early depth testing
                float z_interp = glm::dot(current, depth);
                z_interp = 1 / z_interp;

                if (z_interp < m_DepthBuffer[index + x]) {

                    c.set(glm::dot(current, colorRed) * z_interp, glm::dot(current, colorGreen) * z_interp, glm::dot(current, colorBlue) * z_interp);

                    // set the pixel color
                    m_Framebuffer->set(x, y, c.toTGAColor());

                    // map back to viewport space

                    // update the depth buffer
                   m_DepthBuffer[index + x] = z_interp;
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

        utility::Color c;
        glm::vec3 depth(triangle.position[0].w, triangle.position[1].w, triangle.position[2].w),
            colorRed(triangle.color[0].r, triangle.color[1].r, triangle.color[2].r),
            colorGreen(triangle.color[0].g, triangle.color[1].g, triangle.color[2].g),
            colorBlue(triangle.color[0].b, triangle.color[1].b, triangle.color[2].b);

        for (int y = ymin; y < ymax + 1; y++) {
            int index = y * m_Framebuffer->width();
            for (int x = xmin; x < xmax + 1; x++) {

                // get the barycentric coordinates alpha * P0 + beta * P1 + gamma * P2
                // watch out for orientation!
                glm::vec3 current = barycentric(area, v0, v1, v2, glm::vec2(x, y));

                if (current.x < 0.0f || current.y < 0.0f || current.z < 0.0f)
                    continue;

                // early depth testing
                float z_interp = glm::dot(current, depth);
                
                if (z_interp < m_DepthBuffer[index + x]) {

                    // update the depth buffer
                    m_DepthBuffer[index + x] = z_interp;

                    c.set(glm::dot(current, colorRed), glm::dot(current, colorGreen), glm::dot(current, colorBlue));

                    // set the pixel color
                    m_Framebuffer->set(x, y, c.toTGAColor());
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
        glm::vec3 depth(1 / triangle.position[0].w, 1 / triangle.position[1].w, 1 / triangle.position[2].w),
            textureCoordU(triangle.textureCoord[0].x / triangle.position[0].w,
                triangle.textureCoord[1].x / triangle.position[1].w, triangle.textureCoord[2].x / triangle.position[2].w),
            textureCoordV(triangle.textureCoord[0].y / triangle.position[0].w,
                triangle.textureCoord[1].y / triangle.position[1].w, triangle.textureCoord[2].y / triangle.position[2].w);

        for (int y = ymin; y < ymax + 1; y++) {
            int index = y * m_Framebuffer->width();
            for (int x = xmin; x < xmax + 1; x++) {

                // get the barycentric coordinates alpha * P0 + beta * P1 + gamma * P2
                // watch out for orientation!
                glm::vec3 current = barycentric(area, v0, v1, v2, glm::vec2(x, y));                

                if (current.x < 0.0f || current.y < 0.0f || current.z < 0.0f)
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

                    float u_interp = glm::dot(current, textureCoordU) * z_interp,
                        v_interp = glm::dot(current, textureCoordV) * z_interp,

                        u_interp_right =glm::dot(right, textureCoordU) * z_interp_right,
                        v_interp_right = glm::dot(right, textureCoordV) * z_interp_right,
                        
                        u_interp_up = glm::dot(up, textureCoordU) * z_interp_up,
                        v_interp_up = glm::dot(up, textureCoordV) * z_interp_up;
                    
                    if(x != xmax && y != ymax)
                        mipmapLevel = calcMipmapLevel(u_interp, v_interp, u_interp_right, v_interp_right, u_interp_up, v_interp_up, m_TextureProcessor->getTextureWidth(0), m_TextureProcessor->getTextureHeight(0));

                    // set the pixel color
                    m_Framebuffer->set(x, y, m_TextureProcessor->getTexel(u_interp, v_interp, mipmapLevel));

                    // map back to viewport space
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
        float mipmapLevel = 0;
        glm::vec3 depth(triangle.position[0].z, triangle.position[1].z, triangle.position[2].z),
            textureCoordU(triangle.textureCoord[0].x, triangle.textureCoord[1].x, triangle.textureCoord[2].x),
            textureCoordV(triangle.textureCoord[0].y, triangle.textureCoord[1].y, triangle.textureCoord[2].y);

        // it is an instruction to OpenMP to parallelize the loop since it is perfectly parallel
#pragma omp parallel for
        for (int y = ymin; y < ymax + 1; y++) {
            int index = y * m_Framebuffer->width();
            for (int x = xmin; x < xmax + 1; x++) {

                // get the barycentric coordinates alpha * P0 + beta * P1 + gamma * P2
                // watch out for orientation!
                glm::vec3 current = barycentric(area, v0, v1, v2, glm::vec2(x, y));

                if (current.x < 0.0f || current.y < 0.0f || current.z < 0.0f)
                    continue;

                glm::vec3 right = barycentric(area, v0, v1, v2, glm::vec2(x + 1, y));
                glm::vec3 up = barycentric(area, v0, v1, v2, glm::vec2(x, y + 1));

                // early depth testing
                float z_interp = glm::dot(current, depth);

                if (z_interp < m_DepthBuffer[index + x]) {

                    // update the depth buffer
                    m_DepthBuffer[index + x] = z_interp;

                    float z_interp_right = glm::dot(right, depth),
                        z_interp_up = glm::dot(up, depth);

                    float u_interp = glm::dot(current, textureCoordU),
                        v_interp = glm::dot(current, textureCoordV),

                        u_interp_right = glm::dot(right, textureCoordU),
                        v_interp_right = glm::dot(right, textureCoordV),

                        u_interp_up = glm::dot(up, textureCoordU),
                        v_interp_up = glm::dot(up, textureCoordV);

                    if (x != xmax && y != ymax)
                        mipmapLevel = calcMipmapLevel(u_interp, v_interp, u_interp_right, v_interp_right, u_interp_up, v_interp_up, m_TextureProcessor->getTextureWidth(0), m_TextureProcessor->getTextureHeight(0));

                    // set the pixel color
                    m_Framebuffer->set(x, y, m_TextureProcessor->getTexel(u_interp, v_interp, mipmapLevel));
                }
            }
        }
    }

};