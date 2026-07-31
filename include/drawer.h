#pragma once

#include "utility.h"
#include "texture.h"
#include "frameBuffer.h"

class Rasterizer
{
private:
    float m_Far, m_Near;
    RenderMode m_RenderMode;
    ProjectionMode m_ProjectionMode;
    AttributeMode m_AttributeMode;

 /*   float screenSpaceZ(float z);

    bool isTopLeftEdge(utility::vec2<fp46_16> v0, utility::vec2<fp46_16> v1);

    float calcMipmapLevel(fp46_16 du01, fp46_16 dv01, fp46_16 du10, fp46_16 dv10, fp46_16 width, fp46_16 height);

    void drawSolidOrthoTexture();

    void drawSolid1(Triangle& triangle);

    void drawSolidPerspectiveTexture();

    void drawSolid2(Triangle& triangle);

    void drawSolidPerspectiveColor();

    void drawSolid3(Triangle& triangle);

    void drawSolidOrthoColor();

    void drawSolid4(Triangle& triangle);

    void drawLine(glm::ivec2 v1, glm::ivec2 v2, TGAImage& framebuffer, TGAColor c);

    void drawTriangleWireFrame(glm::ivec2 v1, glm::ivec2 v2, glm::ivec2 v3, TGAImage& framebuffer, TGAColor c);*/

public:
    /*
    void draw(const std::vector<Triangle>& triangles,const Texture& texture, const FrameBuffer& frameBuffer);

    void setFar(float far);

    void setNear(float near);

    void setRenderMode(RenderMode renderMode);

    void setProjectionMode(ProjectionMode projectionMode);

    void setAttributeMode(AttributeMode attributeMode);*/
};