#include "rasterizer.h"

void Rasterizer::draw(const std::vector<Triangle>& triangles, const Texture& texture, FrameBuffer& frameBuffer)
{
    switch (m_RenderMode)
    {
    case RenderMode::WIREFRAME:
        //drawWireframe();
        break;
    case RenderMode::SOLID:
        for (const auto& triangle : triangles)
        {
            drawTextured(triangle, texture, frameBuffer);
        }
        break;
    }
}

void Rasterizer::draw(const std::vector<Triangle>& triangles, FrameBuffer& frameBuffer)
{
    switch (m_RenderMode)
    {
    case RenderMode::WIREFRAME:
        //drawWireframe();
        break;
    case RenderMode::SOLID:
        for (const auto& triangle : triangles)
        {
            drawColored(triangle, frameBuffer);
        }
        break;
    }
}

float Rasterizer::getDistanceFromLine(glm::vec2 point, const glm::vec2& p0, const glm::vec2& p1)
{
    return ((p0.y - p1.y) * point.x + (p1.x - p0.x) * point.y + p0.x * p1.y - p0.y * p1.x);
}

glm::vec3 Rasterizer::getBarycentricCoords(glm::vec2 point, const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& p2)
{
    glm::vec3 barycentricCoords;
    barycentricCoords.x = getDistanceFromLine(point, p1, p2) / getDistanceFromLine(p0, p1, p2);
    barycentricCoords.y = getDistanceFromLine(point, p2, p0) / getDistanceFromLine(p1, p2, p0);
    barycentricCoords.z = 1 - barycentricCoords.x - barycentricCoords.y;

    return barycentricCoords;
}

void Rasterizer::drawTextured(const Triangle& triangle, const Texture& texture, FrameBuffer& frameBuffer)
{
    const glm::vec4& p0 = triangle.position[0];
    const glm::vec4& p1 = triangle.position[1];
    const glm::vec4& p2 = triangle.position[2];
    const glm::vec3& t0 = triangle.textureCoord[0];
    const glm::vec3& t1 = triangle.textureCoord[1];
    const glm::vec3& t2 = triangle.textureCoord[2];

    int xmin = std::max(0.0f, std::min({ p0.x, p1.x, p2.x }));
    int ymin = std::max(0.0f, std::min({ p0.y, p1.y, p2.y }));
    int xmax = std::min(frameBuffer.getWidth() - 1.0f, ceil(std::max({ p0.x, p1.x, p2.x })));
    int ymax = std::min(frameBuffer.getHeight() - 1.0f, ceil(std::max({ p0.y, p1.y, p2.y })));

    auto getTextureCoords = [&](int i, int j, uint8_t& passed) -> glm::vec2
    {
        passed = 1;
        glm::vec2 pixelCenter(i + 0.5, j + 0.5);
        glm::vec3 barycentric = getBarycentricCoords(pixelCenter, glm::vec2(p0), glm::vec2(p1), glm::vec2(p2));

        if (!(barycentric.x >= 0. && barycentric.y >= 0. && barycentric.z >= 0.))
            passed = false;

        float zScreen = glm::dot(glm::vec3(p0.z, p1.z, p2.z), barycentric);
        if (zScreen < frameBuffer.getPixelDepth(i, j))
            frameBuffer.setPixelDepth(i, j, zScreen);
        else
            passed = false;

        glm::vec3 inverseW(1 / p0.w, 1 / p1.w, 1 / p2.w);
        float z = 1. / glm::dot(inverseW, barycentric);

        glm::vec3 barycentricCorrected(z * inverseW.x * barycentric.x, z * inverseW.y * barycentric.y, 0.);
        barycentricCorrected.z = 1 - barycentricCorrected.x - barycentricCorrected.y;

        glm::vec2 textureCoords(glm::dot(glm::vec3(t0.x, t1.x, t2.x), barycentricCorrected), glm::dot(glm::vec3(t0.y, t1.y, t2.y), barycentricCorrected));

        return textureCoords;
    };

    for (int i = xmin; i < xmax + 1; i += 2)
    {
        for (int j = ymin; j < ymax + 1; j += 2)
        {
            uint8_t passed[4];
            int ni = std::min(i + 1, xmax), nj = std::min(j + 1, ymax);
            glm::vec2 tex00, tex10, tex01, tex11;
            tex00 = getTextureCoords(i, j, passed[0]);
            tex10 = getTextureCoords(ni, j, passed[1]);
            tex01 = getTextureCoords(i, nj, passed[2]);
            tex11 = getTextureCoords(ni, nj, passed[3]);

            float mipmapLevel;
            if (passed[0])
            {
                mipmapLevel= texture.getMipmapLevel(tex00, tex10, tex01);
                frameBuffer.setPixelColor(i, j, texture.getTexel(tex00.x, tex00.y, mipmapLevel));
            }

            if (passed[1] && ni != i)
            {
                mipmapLevel = texture.getMipmapLevel(tex10, tex00, tex11);
                frameBuffer.setPixelColor(ni, j, texture.getTexel(tex10.x, tex10.y, mipmapLevel));
            }

            if (passed[2] && nj != j)
            {
                mipmapLevel = texture.getMipmapLevel(tex01, tex11, tex00);
                frameBuffer.setPixelColor(i, nj, texture.getTexel(tex01.x, tex01.y, mipmapLevel));
            }

            if (passed[3] && ni != i && nj != j)
            {
                mipmapLevel = texture.getMipmapLevel(tex11, tex01, tex10);
                frameBuffer.setPixelColor(ni, nj, texture.getTexel(tex11.x, tex11.y, mipmapLevel));
            }
        }
    }
}

void Rasterizer::drawColored(const Triangle& triangle, FrameBuffer& frameBuffer)
{
    const glm::vec4& p0 = triangle.position[0];
    const glm::vec4& p1 = triangle.position[1];
    const glm::vec4& p2 = triangle.position[2];
    const glm::vec3& c0 = triangle.color[0];
    const glm::vec3& c1 = triangle.color[1];
    const glm::vec3& c2 = triangle.color[2];

    int xmin = std::max(0.0f, std::min({ p0.x, p1.x, p2.x }));
    int ymin = std::max(0.0f, std::min({ p0.y, p1.y, p2.y }));
    int xmax = std::min(frameBuffer.getWidth() - 1.0f, ceil(std::max({ p0.x, p1.x, p2.x })));
    int ymax = std::min(frameBuffer.getHeight() - 1.0f, ceil(std::max({ p0.y, p1.y, p2.y })));

    auto getColors = [&](int i, int j, uint8_t& passed) -> glm::vec3
        {
            passed = 1;
            glm::vec2 pixelCenter(i + 0.5, j + 0.5);
            glm::vec3 barycentric = getBarycentricCoords(pixelCenter, glm::vec2(p0), glm::vec2(p1), glm::vec2(p2));

            if (!(barycentric.x >= 0. && barycentric.y >= 0. && barycentric.z >= 0.))
            {
                passed = false;
                return {};
            }

            float zScreen = glm::dot(glm::vec3(p0.z, p1.z, p2.z), barycentric);
            if (zScreen < frameBuffer.getPixelDepth(i, j))
                frameBuffer.setPixelDepth(i, j, zScreen);
            else
            {
                passed = false;
                return {};
            }

            glm::vec3 inverseW(1 / p0.w, 1 / p1.w, 1 / p2.w);
            float z = 1. / glm::dot(inverseW, barycentric);

            glm::vec3 barycentricCorrected(z * inverseW.x * barycentric.x, z * inverseW.y * barycentric.y, 0.);
            barycentricCorrected.z = 1 - barycentricCorrected.x - barycentricCorrected.y;

            glm::vec3 colors(glm::dot(glm::vec3(c0.r, c1.r, c2.r), barycentricCorrected),
                glm::dot(glm::vec3(c0.g, c1.g, c2.g), barycentricCorrected), glm::dot(glm::vec3(c0.b, c1.b, c2.b), barycentricCorrected));

            return colors;
        };

    for (int i = xmin; i < xmax + 1; i += 2)
    {
        for (int j = ymin; j < ymax + 1; j += 2)
        {
            uint8_t passed;
            glm::vec3 color;
            color = getColors(i, j, passed);
            if (passed)
            {
                frameBuffer.setPixelColor(i, j, color);
            }

            if (i + 1 <= xmax)
            {
                color = getColors(i + 1, j, passed);
                if (passed)
                {
                    frameBuffer.setPixelColor(i + 1, j, color);
                }
            }

            if (j + 1 <= ymax)
            {
                color = getColors(i, j + 1, passed);
                if (passed)
                {
                    frameBuffer.setPixelColor(i, j + 1, color);
                }
            }

            if (i + 1 <= xmax && j + 1 <= ymax)
            {
                color = getColors(i + 1, j + 1, passed);
                if (passed)
                {
                    frameBuffer.setPixelColor(i + 1, j + 1, color);
                }
            }

        }
    }
}

/*
bool Drawer::isTopLeftEdge(utility::vec2<fp46_16> v0, utility::vec2<fp46_16> v1) {

    // Assuming CCW winding and positive Y is upward

    // is it a Flat and a Top Edge from v0 to v1 ----- is it a Left Edge from v0 to v1
    return ((v1.y - v0.y == 0 && v1.x - v0.x < 0) || (v1.y < v0.y));
}

void Drawer::drawLine(glm::ivec2 v1, glm::ivec2 v2, TGAImage& framebuffer, TGAColor c) {

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
void Drawer::drawTriangleWireFrame(glm::ivec2 v1, glm::ivec2 v2, glm::ivec2 v3, TGAImage& framebuffer, TGAColor c) {

    drawLine(v1, v2, framebuffer, c);
    drawLine(v2, v3, framebuffer, c);
    drawLine(v3, v1, framebuffer, c);
}
*/

void Rasterizer::setRenderMode(RenderMode renderMode)
{
    m_RenderMode = renderMode;
}