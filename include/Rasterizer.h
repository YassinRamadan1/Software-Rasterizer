#pragma once
#include "utility.h"
#include "texture.h"
#include "frameBuffer.h"

class Rasterizer
{
private:
    RenderMode m_RenderMode;
    WindingOrder m_WindingOrder;
    std::vector<int> m_VerticalIterator;
    /*
    bool isTopLeftEdge(utility::vec2<fp46_16> v0, utility::vec2<fp46_16> v1);
    */
    void drawLine(glm::vec4 p0, glm::vec4 p1, glm::vec3 c0, glm::vec3 c1, FrameBuffer& frameBuffer);

    void drawTriangleWireFramed(const glm::vec4& p0, const glm::vec4& p1, const glm::vec4& p2, const glm::vec3& c0, const glm::vec3& c1, const glm::vec3& c2, FrameBuffer& frameBuffer);
    
    WindingOrder getTriangleWindingOrder(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2);

    float getDistanceFromLine(glm::vec2 point, const glm::vec2& p0, const glm::vec2& p1);
    
    glm::vec2 getBarycentricCoords(glm::vec2 point, const glm::vec2& p0, const glm::vec2& p1);

    glm::vec3 getBarycentricCoords(glm::vec2 point, const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& p2);

    void drawTriangleTextured(const Triangle& triangle, const Texture& texture, FrameBuffer& frameBuffer);

    void drawTriangleColored(const Triangle& triangle, FrameBuffer& frameBuffer);

public:
    void draw(const std::vector<Triangle>& triangles, const Texture& texture, FrameBuffer& frameBuffer);

    void draw(const std::vector<Triangle>& triangles, FrameBuffer& frameBuffer);

    void setRenderMode(RenderMode renderMode);

    void setCulledWindingOrder(WindingOrder windingOrder);
};