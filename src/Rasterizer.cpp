#include "rasterizer.h"
#include <execution>

#define MT 1

void Rasterizer::draw(const std::vector<Triangle>& triangles, const Texture& texture, FrameBuffer& frameBuffer)
{
    m_VerticalIterator.resize((frameBuffer.getHeight() + 1) / 2);
    for (int i = 0; i < (frameBuffer.getHeight() + 1) / 2; ++i)
        m_VerticalIterator[i] = 2 * i;
    for (const auto& triangle : triangles)
    {
        drawTriangleTextured(triangle, texture, frameBuffer);
    }
}

void Rasterizer::draw(const std::vector<Triangle>& triangles, FrameBuffer& frameBuffer)
{
    switch (m_RenderMode)
    {
    case RenderMode::WIREFRAME:
        for (const auto& triangle : triangles)
        {
            drawTriangleWireFramed(triangle.position[0], triangle.position[1], triangle.position[2],
                triangle.color[0], triangle.color[1], triangle.color[2], frameBuffer);
        }
        break;
    case RenderMode::SOLID:
        m_VerticalIterator.resize((frameBuffer.getHeight() + 1) / 2);
        for (int i = 0; i < (frameBuffer.getHeight() + 1) / 2; ++i)
            m_VerticalIterator[i] = 2 * i;

        for (const auto& triangle : triangles)
        {
            drawTriangleColored(triangle, frameBuffer);
        }
        break;
    }
}

WindingOrder Rasterizer::getTriangleWindingOrder(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2)
{
    if ((p1.x - p0.x) * (p2.y - p0.y) - (p1.y - p0.y) * (p2.x - p0.x) > 0)
        return WindingOrder::CCW;
    return WindingOrder::CW;
}

float Rasterizer::getDistanceFromLine(glm::vec2 point, const glm::vec2& p0, const glm::vec2& p1)
{
    return ((p0.y - p1.y) * point.x + (p1.x - p0.x) * point.y + p0.x * p1.y - p0.y * p1.x);
}

glm::vec2 Rasterizer::getBarycentricCoords(glm::vec2 point, const glm::vec2& p0, const glm::vec2& p1)
{
    glm::vec2 coords;
    coords.y = (point.x - p0.x) / (p1.x - p0.x);
    coords.x = 1 - coords.y;
    return coords;
}

glm::vec3 Rasterizer::getBarycentricCoords(glm::vec2 point, const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& p2)
{
    glm::vec3 barycentricCoords;
    barycentricCoords.x = getDistanceFromLine(point, p1, p2) / getDistanceFromLine(p0, p1, p2);
    barycentricCoords.y = getDistanceFromLine(point, p2, p0) / getDistanceFromLine(p1, p2, p0);
    barycentricCoords.z = 1 - barycentricCoords.x - barycentricCoords.y;

    return barycentricCoords;
}

void Rasterizer::drawTriangleTextured(const Triangle& triangle, const Texture& texture, FrameBuffer& frameBuffer)
{
    const glm::vec4& p0 = triangle.position[0];
    const glm::vec4& p1 = triangle.position[1];
    const glm::vec4& p2 = triangle.position[2];
    const glm::vec3& t0 = triangle.textureCoord[0];
    const glm::vec3& t1 = triangle.textureCoord[1];
    const glm::vec3& t2 = triangle.textureCoord[2];

    if (m_WindingOrder == getTriangleWindingOrder(p0, p1, p2))
        return;

    int xmin = std::max(0.0f, std::min({ p0.x, p1.x, p2.x }));
    int ymin = std::max(0.0f, std::min({ p0.y, p1.y, p2.y }));
    int xmax = std::min(frameBuffer.getWidth() - 1.0f, ceil(std::max({ p0.x, p1.x, p2.x })));
    int ymax = std::min(frameBuffer.getHeight() - 1.0f, ceil(std::max({ p0.y, p1.y, p2.y })));

// eleminate the triangle that's entirely off screen
    if (xmax < xmin || ymax < ymin)
        return;

    auto getTextureCoords = [&](int i, int j) -> glm::vec2
    {
        glm::vec2 pixelCenter(i + 0.5, j + 0.5);
        glm::vec3 barycentric = getBarycentricCoords(pixelCenter, glm::vec2(p0), glm::vec2(p1), glm::vec2(p2));

        glm::vec3 inverseW(1 / p0.w, 1 / p1.w, 1 / p2.w);
        float z = 1. / glm::dot(inverseW, barycentric);

        glm::vec3 barycentricCorrected(z * inverseW.x * barycentric.x, z * inverseW.y * barycentric.y, 0.);
        barycentricCorrected.z = 1 - barycentricCorrected.x - barycentricCorrected.y;

        glm::vec2 textureCoords = barycentricCorrected.x * t0 + barycentricCorrected.y * t1 + barycentricCorrected.z * t2;

        return textureCoords;
    };

    int numberOfSamples = frameBuffer.getNumberOfSamples();
    auto samplesPositions = utility::getMSAASamples(static_cast<NumberOfSamples>(numberOfSamples));
    
    auto colorSamples = [&](int i, int j, glm::vec3 color)
    {
        for (int k = 0; k < numberOfSamples; ++k)
        {
            glm::vec2 samplePosition(i + samplesPositions[k].x, j + samplesPositions[k].y);
            glm::vec3 barycentric = getBarycentricCoords(samplePosition, p0, p1, p2);

            if (barycentric.x >= 0. && barycentric.y >= 0. && barycentric.z >= 0.)
            {
                float zScreen = glm::dot(glm::vec3(p0.z, p1.z, p2.z), barycentric);
                if (zScreen < frameBuffer.getSampleDepth(i, j, k))
                {
                    frameBuffer.setSampleDepth(i, j, k, zScreen);
                    frameBuffer.setSampleColor(i, j, k, color);
                }
            }
        }
    };

#if MT
    std::for_each(std::execution::par, m_VerticalIterator.begin() + ymin / 2, m_VerticalIterator.begin() + ymax / 2 + 1,
        [&](int j) {
            
            for (int i = xmin; i < xmax + 1; i += 2)
            {
                int ni = std::min(i + 1, xmax);
                int nj = std::min(j + 1, ymax);
                glm::vec2 tex00, tex10, tex01, tex11;
                tex00 = getTextureCoords(i, j);
                tex10 = getTextureCoords(ni, j);
                tex01 = getTextureCoords(i, nj);
                tex11 = getTextureCoords(ni, nj);

                float mipmapLevel;

                mipmapLevel = texture.getMipmapLevel(tex00, tex10, tex01);
                colorSamples(i, j, texture.getTexel(tex00.x, tex00.y, mipmapLevel));

                if (ni != i)
                {
                    mipmapLevel = texture.getMipmapLevel(tex10, tex00, tex11);
                    colorSamples(ni, j, texture.getTexel(tex10.x, tex10.y, mipmapLevel));
                }

                if (nj != j)
                {
                    mipmapLevel = texture.getMipmapLevel(tex01, tex11, tex00);
                    colorSamples(i, nj, texture.getTexel(tex01.x, tex01.y, mipmapLevel));
                }

                if (ni != i && nj != j)
                {
                    mipmapLevel = texture.getMipmapLevel(tex11, tex01, tex10);
                    colorSamples(ni, nj, texture.getTexel(tex11.x, tex11.y, mipmapLevel));
                }
            }
        });

#else
    for (int i = xmin; i < xmax + 1; i += 2)
    {
        for (int j = ymin; j < ymax + 1; j += 2)
        {
            int ni = std::min(i + 1, xmax);
            int nj = std::min(j + 1, ymax);
            glm::vec2 tex00, tex10, tex01, tex11;
            tex00 = getTextureCoords(i, j);
            tex10 = getTextureCoords(ni, j);
            tex01 = getTextureCoords(i, nj);
            tex11 = getTextureCoords(ni, nj);

            float mipmapLevel;

            mipmapLevel = texture.getMipmapLevel(tex00, tex10, tex01);
            colorSamples(i, j, texture.getTexel(tex00.x, tex00.y, mipmapLevel));
            
            if (ni != i)
            {
                mipmapLevel = texture.getMipmapLevel(tex10, tex00, tex11);
                colorSamples(ni, j, texture.getTexel(tex10.x, tex10.y, mipmapLevel));
            }
            
            if (nj != j)
            {
                mipmapLevel = texture.getMipmapLevel(tex01, tex11, tex00);
                colorSamples(i, nj, texture.getTexel(tex01.x, tex01.y, mipmapLevel));
            }
            
            if (ni != i && nj != j)
            {
                mipmapLevel = texture.getMipmapLevel(tex11, tex01, tex10);
                colorSamples(ni, nj, texture.getTexel(tex11.x, tex11.y, mipmapLevel));
            }
        }
    }
#endif

}

void Rasterizer::drawTriangleColored(const Triangle& triangle, FrameBuffer& frameBuffer)
{
    const glm::vec4& p0 = triangle.position[0];
    const glm::vec4& p1 = triangle.position[1];
    const glm::vec4& p2 = triangle.position[2];
    const glm::vec3& c0 = triangle.color[0];
    const glm::vec3& c1 = triangle.color[1];
    const glm::vec3& c2 = triangle.color[2];

    if (m_WindingOrder == getTriangleWindingOrder(p0, p1, p2))
        return;

    int xmin = std::max(0.0f, std::min({ p0.x, p1.x, p2.x }));
    int ymin = std::max(0.0f, std::min({ p0.y, p1.y, p2.y }));
    int xmax = std::min(frameBuffer.getWidth() - 1.0f, ceil(std::max({ p0.x, p1.x, p2.x })));
    int ymax = std::min(frameBuffer.getHeight() - 1.0f, ceil(std::max({ p0.y, p1.y, p2.y })));

    if (xmax < xmin || ymax < ymin)
        return;

    auto getColor = [&](int i, int j) -> glm::vec3
    {
        glm::vec2 pixelCenter(i + 0.5, j + 0.5);
        glm::vec3 barycentric = getBarycentricCoords(pixelCenter, glm::vec2(p0), glm::vec2(p1), glm::vec2(p2));

        glm::vec3 inverseW(1 / p0.w, 1 / p1.w, 1 / p2.w);
        float z = 1. / glm::dot(inverseW, barycentric);

        glm::vec3 barycentricCorrected(z * inverseW.x * barycentric.x, z * inverseW.y * barycentric.y, 0.);
        barycentricCorrected.z = 1 - barycentricCorrected.x - barycentricCorrected.y;

        glm::vec3 colors = barycentricCorrected.x * c0 + barycentricCorrected.y * c1 + barycentricCorrected.z * c2;

        return colors;
    };

    int numberOfSamples = frameBuffer.getNumberOfSamples();
    auto samplesPositions = utility::getMSAASamples(static_cast<NumberOfSamples>(numberOfSamples));

    auto colorSamples = [&](int i, int j, glm::vec3 color)
    {
        for (int k = 0; k < numberOfSamples; ++k)
        {
            glm::vec2 samplePosition(i + samplesPositions[k].x, j + samplesPositions[k].y);
            glm::vec3 barycentric = getBarycentricCoords(samplePosition, p0, p1, p2);

            if (barycentric.x >= 0. && barycentric.y >= 0. && barycentric.z >= 0.)
            {
                float zScreen = glm::dot(glm::vec3(p0.z, p1.z, p2.z), barycentric);
                if (zScreen < frameBuffer.getSampleDepth(i, j, k))
                {
                    frameBuffer.setSampleDepth(i, j, k, zScreen);
                    frameBuffer.setSampleColor(i, j, k, color);
                }
            }
        }
    };

#if MT
    std::for_each(std::execution::par, m_VerticalIterator.begin() + ymin / 2, m_VerticalIterator.begin() + ymax / 2 + 1,
        [&](int j) {

            for (int i = xmin; i < xmax + 1; i += 2)
            {
                int ni = std::min(i + 1, xmax);
                int nj = std::min(j + 1, ymax);

                colorSamples(i, j, getColor(i, j));

                if (ni != i)
                {
                    colorSamples(ni, j, getColor(ni, j));
                }

                if (nj != j)
                {
                    colorSamples(i, nj, getColor(i, nj));
                }

                if (ni != i && nj != j)
                {
                    colorSamples(ni, nj, getColor(ni, nj));
                }
            }
        });
#else
    for (int i = xmin; i < xmax + 1; i += 2)
    {
        for (int j = ymin; j < ymax + 1; j += 2)
        {
            int ni = std::min(i + 1, xmax);
            int nj = std::min(j + 1, ymax);

            colorSamples(i, j, getColor(i, j));

            if (ni != i)
            {
                colorSamples(ni, j, getColor(ni, j));
            }

            if (nj != j)
            {
                colorSamples(i, nj, getColor(i, nj));
            }

            if (ni != i && nj != j)
            {
                colorSamples(ni, nj, getColor(ni, nj));
            }
        }
    }
#endif

}

/*
bool Drawer::isTopLeftEdge(utility::vec2<fp46_16> v0, utility::vec2<fp46_16> v1) {

    // Assuming CCW winding and positive Y is upward

    // is it a Flat and a Top Edge from v0 to v1 ----- is it a Left Edge from v0 to v1
    return ((v1.y - v0.y == 0 && v1.x - v0.x < 0) || (v1.y < v0.y));
}
*/

void Rasterizer::drawLine(glm::vec4 p0, glm::vec4 p1, glm::vec3 c0, glm::vec3 c1, FrameBuffer& frameBuffer)
{
    bool isTransposed = false;
    if (abs(p1.x - p0.x) < abs(p1.y - p0.y)) // transpose
    {  
        std::swap(p0.x, p0.y);
        std::swap(p1.x, p1.y);
        isTransposed = true;
    }
    if (p1.x - p0.x < 0) // draw from left to right
    {
        std::swap(p0, p1);
        std::swap(c0, c1);
    }

    glm::vec2 p(glm::floor(p0));
    c0 /= p0.w, c1 /= p1.w;
    glm::vec3 c;
    glm::vec2 barycentric;

    int numberOfSamples = frameBuffer.getNumberOfSamples();
    auto samplesPositions = utility::getMSAASamples(static_cast<NumberOfSamples>(numberOfSamples));

    auto f = [&](glm::vec2 p)
    {
        if (isTransposed)
        {
            if (!(p.x < frameBuffer.getHeight() && p.y < frameBuffer.getWidth() && p.x >= 0 && p.y >= 0))
                return;

            for (int i = 0; i < numberOfSamples; ++i)
            {
                glm::vec2 sample = p + samplesPositions[i];
                barycentric = getBarycentricCoords(sample, p0, p1);
                float zScreen = glm::dot(glm::vec2(p0.z, p1.z), barycentric);
                if (zScreen < frameBuffer.getSampleDepth(p.y, p.x, i))
                {
                    frameBuffer.setSampleDepth(p.y, p.x, i, zScreen);

                    float z = 1 / glm::dot(glm::vec2(1 / p0.w, 1 / p1.w), barycentric);
                    c = z * (barycentric.x * c0 + barycentric.y * c1);

                    frameBuffer.setSampleColor(p.y, p.x, i, c);
                }
            }

        }
        else
        {
            if (!(p.x < frameBuffer.getWidth() && p.y < frameBuffer.getHeight() && p.x >= 0 && p.y >= 0))
                return;
            for (int i = 0; i < numberOfSamples; ++i)
            {
                glm::vec2 sample = p + samplesPositions[i];
                barycentric = getBarycentricCoords(sample, p0, p1);
                float zScreen = glm::dot(glm::vec2(p0.z, p1.z), barycentric);
                if (zScreen < frameBuffer.getSampleDepth(p.x, p.y, i))
                {
                    frameBuffer.setSampleDepth(p.x, p.y, i, zScreen);
                    float z = 1 / glm::dot(glm::vec2(1 / p0.w, 1 / p1.w), barycentric);
                    c = z * (barycentric.x * c0 + barycentric.y * c1);

                    frameBuffer.setSampleColor(p.x, p.y, i, c);
                }
            }
        }
    };

    f(p + glm::vec2(0.5));
    if (p1.y - p0.y >= 0)
    {
        while (p.x <= p1.x)
        {
            float d = getDistanceFromLine(p + glm::vec2(1.5, 1), p0, p1);
            if (d > 0)
                f(p + glm::vec2(1.5, 0.5));
            else
            {
                f(p + glm::vec2(1.5));
                ++p.y;
            }
            ++p.x;
        }
    }
    else
    {
        while (p.x <= p1.x)
        {
            float d = getDistanceFromLine(p + glm::vec2(1.5, 0.), p0, p1);
            if (d > 0)
            {
                f(p + glm::vec2(1.5, -0.5));
                --p.y;
            }
            else
                f(p + glm::vec2(1.5, 0.5));
            ++p.x;
        }
    }
}

void Rasterizer::drawTriangleWireFramed(const glm::vec4& p0, const glm::vec4& p1, const glm::vec4& p2, const glm::vec3& c0, const glm::vec3& c1, const glm::vec3& c2, FrameBuffer& frameBuffer)
{
    if (m_WindingOrder == getTriangleWindingOrder(p0, p1, p2))
        return;

    drawLine(p0, p1, c0, c1, frameBuffer);
    drawLine(p1, p2, c1, c2, frameBuffer);
    drawLine(p2, p0, c2, c0, frameBuffer);
}

void Rasterizer::setRenderMode(RenderMode renderMode)
{
    m_RenderMode = renderMode;
}

void Rasterizer::setCulledWindingOrder(WindingOrder windingOrder)
{
    m_WindingOrder = windingOrder;
}