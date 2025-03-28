#pragma once

#include "utility.h"
class Drawer {

private:

    std::vector<Triangle>* m_Triangles;
    TextureProcessor* m_TextureProcessor;
    TGAImage* m_Framebuffer;
    float m_Far, m_Near, m_Viewport_Near = 0, m_Viewport_Far = 1, ** m_DepthBuffer;
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

    void setDepthBuffer(float**& depthBuffer) {
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

    float screenSpaceZ(float z) {

        return m_Viewport_Near + (z - m_Near) * (m_Viewport_Far - m_Viewport_Near) / (m_Far - m_Near);
    }

    bool isTopLeftEdge(utility::vec2<fp46_16> v0, utility::vec2<fp46_16> v1) {

        // Assuming CCW winding and positive Y is upward

        // is it a Flat and a Top Edge from v0 to v1 ----- is it a Left Edge from v0 to v1
        return ((v1.y - v0.y == 0 && v1.x - v0.x < 0) || (v1.y < v0.y));
    }

    float calcMipmapLevel(fp46_16 du01, fp46_16 dv01, fp46_16 du10, fp46_16 dv10, fp46_16 width, fp46_16 height) {

        du01 *= width, dv01 *= height, du10 *= width, dv10 *= height;
        float L = double(std::max(du01 * du01 + dv01 * dv01, du10 * du10 + dv10 * dv10));

        return std::log2(L) / 2.0f;
    }

    void drawSolidOrthoTexture() {
        for (auto& triangle : (*m_Triangles)) {

            drawSolid1(triangle);
        }
    }

    void drawSolid1(Triangle& triangle) {

        utility::vec2<fp46_16> v0 = utility::vec2<fp46_16>(triangle.position[0].x, triangle.position[0].y),
            v1 = utility::vec2<fp46_16>(triangle.position[1].x, triangle.position[1].y),
            v2 = utility::vec2<fp46_16>(triangle.position[2].x, triangle.position[2].y);

        float z_value, mipmapLevel = 0;
        fp46_16 z_interp, z_interp_right, z_interp_up, u_interp, v_interp, u_interp_right, v_interp_right, u_interp_up, v_interp_up,
            zero(0.0f), one(1.0f), bias, w(m_TextureProcessor->getTextureWidth(0)), h(m_TextureProcessor->getTextureHeight(0)), area = utility::cross(v0, v1, v2);

        // we set the minimum unit in the fixed point
        bias.setULP();

        // if CW winding then reverse the order of v1 and v2
        if (area < 0) {
            std::swap(v1, v2),
                std::swap(triangle.position[1], triangle.position[2]),
                std::swap(triangle.color[1], triangle.color[2]),
                std::swap(triangle.textureCoord[1], triangle.textureCoord[2]);
            area = -area;
        }

        // get the bounding box of the triangle and clip it to the framebuffer (clip against the left, right, bottom and top planes)
        int xmin = std::max(0.0f, std::min(triangle.position[0].x, std::min(triangle.position[1].x, triangle.position[2].x))),
            ymin = std::max(0.0f, std::min(triangle.position[0].y, std::min(triangle.position[1].y, triangle.position[2].y))),
            xmax = std::min(m_Framebuffer->width() - 1.0f, ceil(std::max(triangle.position[0].x, std::max(triangle.position[1].x, triangle.position[2].x)))),
            ymax = std::min(m_Framebuffer->height() - 1.0f, ceil(std::max(triangle.position[0].y, std::max(triangle.position[1].y, triangle.position[2].y))));

        utility::vec2<fp46_16> p(fp46_16(xmin + 0.5f), fp46_16(ymin + 0.5f));

        utility::vec3<fp46_16> current, right, up,

            // To Follow the BottomRight tie breaking rule
            initialCrosses(utility::cross(v1, v2, p) - (isTopLeftEdge(v1, v2) ? bias : zero),
                utility::cross(v2, v0, p) - (isTopLeftEdge(v2, v0) ? bias : zero),
                utility::cross(v0, v1, p) - (isTopLeftEdge(v0, v1) ? bias : zero)), currentCrosses = initialCrosses,

            dx(v1.y - v2.y, v2.y - v0.y, v0.y - v1.y), dy(v2.x - v1.x, v0.x - v2.x, v1.x - v0.x),


            depth(fp46_16(triangle.position[0].z), fp46_16(triangle.position[1].z), fp46_16(triangle.position[2].z)),

            textureCoordU(fp46_16(triangle.textureCoord[0].x), fp46_16(triangle.textureCoord[1].x), fp46_16(triangle.textureCoord[2].x)),

            textureCoordV(fp46_16(triangle.textureCoord[0].y), fp46_16(triangle.textureCoord[1].y), fp46_16(triangle.textureCoord[2].y));

        for (int y = ymin; y < ymax + 1; y++) {
            for (int x = xmin; x < xmax + 1; x++) {

                // get the barycentric coordinates alpha * P0 + beta * P1 + gamma * P2
                // watch out for orientation!

                current = utility::vec3(currentCrosses.x / area, currentCrosses.y / area, currentCrosses.z / area);

                if (current.x < zero || current.y < zero || current.z < zero) {

                    currentCrosses = currentCrosses + dx;
                    continue;
                };

                // early depth testing
                z_interp = utility::dot(current, depth);
                z_value = double(z_interp);

                if (z_value < m_DepthBuffer[y][x]) {

                    right = utility::vec3((currentCrosses.x + dx.x) / area, (currentCrosses.y + dx.y) / area, (currentCrosses.z + dx.z) / area);
                    up = utility::vec3((currentCrosses.x + dy.x) / area, (currentCrosses.y + dy.y) / area, (currentCrosses.z + dy.z) / area);

                    u_interp = utility::dot(current, textureCoordU), v_interp = utility::dot(current, textureCoordV),

                        u_interp_right = utility::dot(right, textureCoordU), v_interp_right = utility::dot(right, textureCoordV),

                        u_interp_up = utility::dot(up, textureCoordU), v_interp_up = utility::dot(up, textureCoordV);

                    if (x != xmax && y != ymax)
                        mipmapLevel = calcMipmapLevel(u_interp_right - u_interp, v_interp_right - v_interp, u_interp_up - u_interp, v_interp_up - v_interp, w, h);

                    // set the pixel color
                    m_Framebuffer->set(x, y, m_TextureProcessor->getTexel(double(u_interp), double(v_interp), mipmapLevel));

                    // update the depth buffer
                    m_DepthBuffer[y][x] = z_value;
                }

                // update the cross products
                currentCrosses = currentCrosses + dx;
            }

            // update the cross products to start from the next scanline from the xmin value
            initialCrosses = initialCrosses + dy;
            currentCrosses = initialCrosses;
        }
    }

    void drawSolidPerspectiveTexture() {
        for (auto& triangle : (*m_Triangles)) {

            drawSolid2(triangle);
        }
    }

    void drawSolid2(Triangle& triangle) {

        utility::vec2<fp46_16> v0 = utility::vec2<fp46_16>(triangle.position[0].x, triangle.position[0].y),
            v1 = utility::vec2<fp46_16>(triangle.position[1].x, triangle.position[1].y),
            v2 = utility::vec2<fp46_16>(triangle.position[2].x, triangle.position[2].y);

        float z_value, mipmapLevel = 0;
        fp46_16 z_interp, z_interp_right, z_interp_up, u_interp, v_interp, u_interp_right, v_interp_right, u_interp_up, v_interp_up,
            zero(0.0f), one(1.0f), bias, w(m_TextureProcessor->getTextureWidth(0)), h(m_TextureProcessor->getTextureHeight(0)), area = utility::cross(v0, v1, v2);
        
        // we set the minimum unit in the fixed point
        bias.setULP();
        
        // if CW winding then reverse the order of v1 and v2
        if (area < 0) {
            std::swap(v1, v2),
                std::swap(triangle.position[1], triangle.position[2]),
                std::swap(triangle.color[1], triangle.color[2]),
                std::swap(triangle.textureCoord[1], triangle.textureCoord[2]);
            area = -area;
        }

        // get the bounding box of the triangle and clip it to the framebuffer (clip against the left, right, bottom and top planes)

        int xmin = std::max(0.0f, std::min(triangle.position[0].x, std::min(triangle.position[1].x, triangle.position[2].x))),
            ymin = std::max(0.0f, std::min(triangle.position[0].y, std::min(triangle.position[1].y, triangle.position[2].y))),
            xmax = std::min(m_Framebuffer->width() - 1.0f, ceil(std::max(triangle.position[0].x, std::max(triangle.position[1].x, triangle.position[2].x)))),
            ymax = std::min(m_Framebuffer->height() - 1.0f, ceil(std::max(triangle.position[0].y, std::max(triangle.position[1].y, triangle.position[2].y))));

        utility::vec2<fp46_16> p(fp46_16(xmin + 0.5f), fp46_16(ymin + 0.5f));

        utility::vec3<fp46_16> current, right, up,

            // To Follow the BottomRight tie breaking rule
            initialCrosses(utility::cross(v1, v2, p) - (isTopLeftEdge(v1, v2) ? bias : zero),
                utility::cross(v2, v0, p) - (isTopLeftEdge(v2, v0) ? bias : zero),
                utility::cross(v0, v1, p) - (isTopLeftEdge(v0, v1) ? bias : zero)), currentCrosses = initialCrosses,

            dx(v1.y - v2.y, v2.y - v0.y, v0.y - v1.y), dy(v2.x - v1.x, v0.x - v2.x, v1.x - v0.x),

            depth(fp46_16(1 / triangle.position[0].w), fp46_16(1 / triangle.position[1].w), fp46_16(1 / triangle.position[2].w)),

            textureCoordU(fp46_16(triangle.textureCoord[0].x / triangle.position[0].w),
                fp46_16(triangle.textureCoord[1].x / triangle.position[1].w), fp46_16(triangle.textureCoord[2].x / triangle.position[2].w)),

            textureCoordV(fp46_16(triangle.textureCoord[0].y / triangle.position[0].w),
                fp46_16(triangle.textureCoord[1].y / triangle.position[1].w), fp46_16(triangle.textureCoord[2].y / triangle.position[2].w));

        for (int y = ymin; y < ymax + 1; y++) {

            for (int x = xmin; x < xmax + 1; x++) {

                // get the barycentric coordinates alpha * P0 + beta * P1 + gamma * P2
                // watch out for orientation!

                current = utility::vec3(currentCrosses.x / area, currentCrosses.y / area, currentCrosses.z / area);
                
                if (current.x < zero || current.y < zero || current.z < zero) {

                    currentCrosses = currentCrosses + dx;
                    continue;
                }

                // early depth testing
                z_interp = utility::dot(current, depth);
                z_interp = one / z_interp;
                z_value = screenSpaceZ(double(z_interp));

                if (z_value < m_DepthBuffer[y][x]) {

                    right = utility::vec3((currentCrosses.x + dx.x) / area, (currentCrosses.y + dx.y) / area, (currentCrosses.z + dx.z) / area);
                    up = utility::vec3((currentCrosses.x + dy.x) / area, (currentCrosses.y + dy.y) / area, (currentCrosses.z + dy.z) / area);

                    z_interp_right = utility::dot(right, depth),
                        z_interp_up = utility::dot(up, depth);
                    z_interp_right = one / z_interp_right, z_interp_up = one / z_interp_up;

                    u_interp = utility::dot(current, textureCoordU) * z_interp, v_interp = utility::dot(current, textureCoordV) * z_interp,

                        u_interp_right = utility::dot(right, textureCoordU) * z_interp_right, v_interp_right = utility::dot(right, textureCoordV) * z_interp_right,

                        u_interp_up = utility::dot(up, textureCoordU) * z_interp_up, v_interp_up = utility::dot(up, textureCoordV) * z_interp_up;

                    if (x != xmax && y != ymax)
                        mipmapLevel = calcMipmapLevel(u_interp_right - u_interp, v_interp_right - v_interp, u_interp_up - u_interp, v_interp_up - v_interp, w, h);

                    // set the pixel color
                    m_Framebuffer->set(x, y, m_TextureProcessor->getTexel(double(u_interp), double(v_interp), mipmapLevel));

                    // update the depth buffer
                    m_DepthBuffer[y][x] = z_value;
                }

                // update the cross products
                currentCrosses = currentCrosses + dx;
            }

            // update the cross products to start from the next scanline from the xmin value
            initialCrosses = initialCrosses + dy;
            currentCrosses = initialCrosses;
        }
    }

    void drawSolidPerspectiveColor() {
        for (auto& triangle : (*m_Triangles)) {

            drawSolid3(triangle);
        }
    }

    void drawSolid3(Triangle& triangle) {

        utility::vec2<fp46_16> v0 = utility::vec2<fp46_16>(triangle.position[0].x, triangle.position[0].y),
            v1 = utility::vec2<fp46_16>(triangle.position[1].x, triangle.position[1].y),
            v2 = utility::vec2<fp46_16>(triangle.position[2].x, triangle.position[2].y);

        float z_value, mipmapLevel = 0;
        fp46_16 z_interp, r_interp, g_interp, b_interp, zero(0.0f), one(1.0f), bias, area = utility::cross(v0, v1, v2);
        
        // we set the minimum unit in the fixed point
        bias.setULP();
        utility::Color c1;
        // if CW winding then reverse the order of v1 and v2
        if (area < 0) {
            std::swap(v1, v2),
                std::swap(triangle.position[1], triangle.position[2]),
                std::swap(triangle.color[1], triangle.color[2]),
                std::swap(triangle.textureCoord[1], triangle.textureCoord[2]);
            area = -area;
        }

        // get the bounding box of the triangle and clip it to the framebuffer (clip against the left, right, bottom and top planes)
        int xmin = std::max(0.0f, std::min(triangle.position[0].x, std::min(triangle.position[1].x, triangle.position[2].x))),
            ymin = std::max(0.0f, std::min(triangle.position[0].y, std::min(triangle.position[1].y, triangle.position[2].y))),
            xmax = std::min(m_Framebuffer->width() - 1.0f, ceil(std::max(triangle.position[0].x, std::max(triangle.position[1].x, triangle.position[2].x)))),
            ymax = std::min(m_Framebuffer->height() - 1.0f, ceil(std::max(triangle.position[0].y, std::max(triangle.position[1].y, triangle.position[2].y))));

        utility::vec2<fp46_16> p(fp46_16(xmin + 0.5f), fp46_16(ymin + 0.5f));

        utility::vec3<fp46_16> current,

            // To Follow the BottomRight tie breaking rule
            initialCrosses(utility::cross(v1, v2, p) - (isTopLeftEdge(v1, v2) ? bias : zero),
                utility::cross(v2, v0, p) - (isTopLeftEdge(v2, v0) ? bias : zero),
                utility::cross(v0, v1, p) - (isTopLeftEdge(v0, v1) ? bias : zero)), currentCrosses = initialCrosses,

            dx(v1.y - v2.y, v2.y - v0.y, v0.y - v1.y), dy(v2.x - v1.x, v0.x - v2.x, v1.x - v0.x),

            depth(fp46_16(1 / triangle.position[0].w), fp46_16(1 / triangle.position[1].w), fp46_16(1 / triangle.position[2].w)),

            colorRed(fp46_16(triangle.color[0].r / triangle.position[0].w),
                fp46_16(triangle.color[1].r / triangle.position[1].w), fp46_16(triangle.color[2].r / triangle.position[2].w)),

            colorGreen(fp46_16(triangle.color[0].g / triangle.position[0].w),
                fp46_16(triangle.color[1].g / triangle.position[1].w), fp46_16(triangle.color[2].g / triangle.position[2].w)),

            colorBlue(fp46_16(triangle.color[0].b / triangle.position[0].w),
                fp46_16(triangle.color[1].b / triangle.position[1].w), fp46_16(triangle.color[2].b / triangle.position[2].w));

        for (int y = ymin; y < ymax + 1; y++) {
            for (int x = xmin; x < xmax + 1; x++) {

                // get the barycentric coordinates alpha * P0 + beta * P1 + gamma * P2
                // watch out for orientation!

                current = utility::vec3(currentCrosses.x / area, currentCrosses.y / area, currentCrosses.z / area);

                if (current.x < zero || current.y < zero || current.z < zero) {

                    currentCrosses = currentCrosses + dx;
                    continue;
                }

                // early depth testing
                z_interp = utility::dot(current, depth);
                z_interp = one / z_interp;
                z_value = screenSpaceZ(double(z_interp));

                if (z_value < m_DepthBuffer[y][x]) {

                    r_interp = utility::dot(current, colorRed) * z_interp, g_interp = utility::dot(current, colorGreen) * z_interp, b_interp = utility::dot(current, colorBlue) * z_interp;
                    c1.set(double(r_interp), double(g_interp), double(b_interp));
                    // set the pixel color
                    m_Framebuffer->set(x, y, c1.toTGAColor());

                    // update the depth buffer
                    m_DepthBuffer[y][x] = z_value;
                }

                // update the cross products
                currentCrosses = currentCrosses + dx;
            }

            // update the cross products to start from the next scanline from the xmin value
            initialCrosses = initialCrosses + dy;
            currentCrosses = initialCrosses;
        }
    }

    void drawSolidOrthoColor() {
        for (auto& triangle : (*m_Triangles)) {

            drawSolid4(triangle);
        }
    }

    void drawSolid4(Triangle& triangle) {

        utility::vec2<fp46_16> v0 = utility::vec2<fp46_16>(triangle.position[0].x, triangle.position[0].y),
            v1 = utility::vec2<fp46_16>(triangle.position[1].x, triangle.position[1].y),
            v2 = utility::vec2<fp46_16>(triangle.position[2].x, triangle.position[2].y);

        float z_value, mipmapLevel = 0;
        fp46_16 z_interp, r_interp, g_interp, b_interp, zero(0.0f), bias, area = utility::cross(v0, v1, v2);
        
        // we set the minimum unit in the fixed point
        bias.setULP();
        utility::Color c1;
        // if CW winding then reverse the order of v1 and v2
        if (area < 0) {
            std::swap(v1, v2),
                std::swap(triangle.position[1], triangle.position[2]),
                std::swap(triangle.color[1], triangle.color[2]),
                std::swap(triangle.textureCoord[1], triangle.textureCoord[2]);
            area = -area;
        }

        // get the bounding box of the triangle and clip it to the framebuffer (clip against the left, right, bottom and top planes)
        int xmin = std::max(0.0f, std::min(triangle.position[0].x, std::min(triangle.position[1].x, triangle.position[2].x))),
            ymin = std::max(0.0f, std::min(triangle.position[0].y, std::min(triangle.position[1].y, triangle.position[2].y))),
            xmax = std::min(m_Framebuffer->width() - 1.0f, ceil(std::max(triangle.position[0].x, std::max(triangle.position[1].x, triangle.position[2].x)))),
            ymax = std::min(m_Framebuffer->height() - 1.0f, ceil(std::max(triangle.position[0].y, std::max(triangle.position[1].y, triangle.position[2].y))));

        utility::vec2<fp46_16> p(fp46_16(xmin + 0.5f), fp46_16(ymin + 0.5f));

        utility::vec3<fp46_16> current,
            
            // To Follow the BottomRight tie breaking rule
            initialCrosses(utility::cross(v1, v2, p) - (isTopLeftEdge(v1, v2) ? bias : zero),
                utility::cross(v2, v0, p) - (isTopLeftEdge(v2, v0) ? bias : zero),
                utility::cross(v0, v1, p) - (isTopLeftEdge(v0, v1) ? bias : zero)), currentCrosses = initialCrosses,

            dx(v1.y - v2.y, v2.y - v0.y, v0.y - v1.y), dy(v2.x - v1.x, v0.x - v2.x, v1.x - v0.x),

            depth(fp46_16(triangle.position[0].z), fp46_16(triangle.position[1].z), fp46_16(triangle.position[2].z)),

            colorRed(fp46_16(triangle.color[0].r), fp46_16(triangle.color[1].r), fp46_16(triangle.color[2].r)),

            colorGreen(fp46_16(triangle.color[0].g), fp46_16(triangle.color[1].g), fp46_16(triangle.color[2].g)),

            colorBlue(fp46_16(triangle.color[0].b), fp46_16(triangle.color[1].b), fp46_16(triangle.color[2].b));

        for (int y = ymin; y < ymax + 1; y++) {
            for (int x = xmin; x < xmax + 1; x++) {

                // get the barycentric coordinates alpha * P0 + beta * P1 + gamma * P2
                // watch out for orientation!

                current = utility::vec3(currentCrosses.x / area, currentCrosses.y / area, currentCrosses.z / area);

                if (current.x < zero || current.y < zero || current.z < zero) {

                    currentCrosses = currentCrosses + dx;
                    continue;
                }

                // early depth testing
                z_interp = utility::dot(current, depth);
                z_value = double(z_interp);

                if (z_value < m_DepthBuffer[y][x]) {

                    r_interp = utility::dot(current, colorRed), g_interp = utility::dot(current, colorGreen),
                        b_interp = utility::dot(current, colorBlue), c1.set(double(r_interp), double(g_interp), double(b_interp));

                    // set the pixel color
                    m_Framebuffer->set(x, y, c1.toTGAColor());

                    // update the depth buffer
                    m_DepthBuffer[y][x] = z_value;
                }

                // update the cross products
                currentCrosses = currentCrosses + dx;
            }

            // update the cross products to start from the next scanline from the xmin value
            initialCrosses = initialCrosses + dy;
            currentCrosses = initialCrosses;
        }
    }

    void drawLine(glm::ivec2 v1, glm::ivec2 v2, TGAImage& framebuffer, TGAColor c) {

        bool isTransposed = false;
        if (abs(v2.x - v1.x) < abs(v2.y - v1.y)) { // transpose 
            std::swap(v1.x, v1.y);
            std::swap(v2.x, v2.y);
            isTransposed = true;
        }
        if (v2.x - v1.x < 0) { // draw from left to right
            std::swap(v1.x, v2.x);
            std::swap(v1.y, v2.y);
        }

        int dx = v2.x - v1.x, dy = v2.y - v1.y, x = v1.x, y = v1.y;

        if (isTransposed)
            framebuffer.set(y, x, c);
        else
            framebuffer.set(x, y, c);

        if (dy < 0) {

            int d = -dx - 2 * dy, d1 = -2 * dy, d2 = -2 * (dx + dy);

            while (x < v2.x) {
                x++;
                if (d < 0)
                    d += d1;
                else
                    y--, d += d2;
                if (isTransposed)
                    framebuffer.set(y, x, c);
                else
                    framebuffer.set(x, y, c);
            }
        }
        else {

            int d = dx - 2 * dy, d1 = -2 * dy, d2 = 2 * (dx - dy);
            while (x < v2.x) {
                x++;
                if (d > 0)
                    d += d1;
                else
                    y++, d += d2;
                if (isTransposed)
                    framebuffer.set(y, x, c);
                else
                    framebuffer.set(x, y, c);
            }
        }
    }

    void drawTriangleWireFrame(glm::ivec2 v1, glm::ivec2 v2, glm::ivec2 v3, TGAImage& framebuffer, TGAColor c) {

        drawLine(v1, v2, framebuffer, c);
        drawLine(v2, v3, framebuffer, c);
        drawLine(v3, v1, framebuffer, c);
    }
};