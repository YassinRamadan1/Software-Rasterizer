#pragma once
#include "utility.h"
#include "texture.h"
#include "frameBuffer.h"

class Rasterizer
{
private:
    RenderMode m_RenderMode;
    /*
    bool isTopLeftEdge(utility::vec2<fp46_16> v0, utility::vec2<fp46_16> v1);

    void drawSolidOrthoTexture();

    void drawSolid1(Triangle& triangle);

    void drawSolidPerspectiveTexture();

    void drawSolid2(Triangle& triangle);

    void drawSolidPerspectiveColor();

    void drawSolid3(Triangle& triangle);

    void drawSolidOrthoColor();

    void drawSolid4(Triangle& triangle);

    void drawLine(glm::ivec2 v1, glm::ivec2 v2, TGAImage& framebuffer, TGAColor c);

    void drawTriangleWireFrame(glm::ivec2 v1, glm::ivec2 v2, glm::ivec2 v3, TGAImage& framebuffer, TGAColor c);
    */
    float getDistanceFromLine(glm::vec2 point, const glm::vec2& p0, const glm::vec2& p1);
    
    glm::vec3 getBarycentricCoords(glm::vec2 point, const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& p2);

    void drawTextured(const Triangle& triangle, const Texture& texture, FrameBuffer& frameBuffer);

    void drawColored(const Triangle& triangle, FrameBuffer& frameBuffer);

public:
    void draw(const std::vector<Triangle>& triangles, const Texture& texture, FrameBuffer& frameBuffer);

    void draw(const std::vector<Triangle>& triangles, FrameBuffer& frameBuffer);

    void setRenderMode(RenderMode renderMode);
};