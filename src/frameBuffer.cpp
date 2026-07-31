#include "FrameBuffer.h"

FrameBuffer::FrameBuffer(int width, int height, glm::vec3 fillColor, float fillDepth)
	: m_Width(width), m_Height(height), m_ColorBuffer(width, height, 4), m_FillDepth(fillDepth), m_FillColor(utility::toTGAColor(fillColor))
{
	m_DepthBuffer = new float* [m_Width];

	for (int i = 0; i < m_Width; ++i) {
		m_DepthBuffer[i] = new float[m_Height];
		for (int j = 0; j < m_Height; ++j)
		{
			m_DepthBuffer[i][j] = m_FillDepth;
			m_ColorBuffer.set(i, j, m_FillColor);
		}
	}
}

bool FrameBuffer::loadColorBuffer(const std::string filename)
{
	return m_ColorBuffer.read_tga_file(filename);
}

bool FrameBuffer::storeColorBuffer(const std::string filename, const bool vflip, const bool rle)
{
	return m_ColorBuffer.write_tga_file(filename, vflip, rle);
}

bool FrameBuffer::storeDepthBuffer(const std::string filename, const bool vflip, const bool rle)
{
	TGAImage image(m_Width, m_Height, TGAImage::GRAYSCALE);

	return image.write_tga_file(filename, vflip, rle);
}

void FrameBuffer::flipHorizontally()
{	
	m_ColorBuffer.flip_horizontally();

	for (int i = 0; i < m_Height; ++i) {
		for (int j = 0; j < m_Width / 2; ++j)
			std::swap(m_DepthBuffer[j][i], m_DepthBuffer[m_Width - j - 1][i]);
	}
}


void FrameBuffer::flipVertically()
{
	m_ColorBuffer.flip_vertically();

	for (int i = 0; i < m_Width; ++i) {
		for (int j = 0; j < m_Height / 2; ++j)
			std::swap(m_DepthBuffer[i][j], m_DepthBuffer[i][m_Height - j - 1]);
	}
}

void FrameBuffer::setFillColor(glm::vec3 color)
{
	m_FillColor = utility::toTGAColor(color);
}

void FrameBuffer::setFillDepth(float depth)
{
	m_FillDepth = depth;
}

void FrameBuffer::setPixelColor(int i, int j, glm::vec3 color)
{
	m_ColorBuffer.set(i, j, utility::toTGAColor(color));
}

glm::vec3 FrameBuffer::getPixelColor(int i, int j) const
{
	return utility::toColor(m_ColorBuffer.get(i, j));
}

void FrameBuffer::setPixelDepth(int i, int j, float depth)
{
	m_DepthBuffer[i][j] = depth;
}

float FrameBuffer::getPixelDepth(int i, int j) const
{
	return m_DepthBuffer[i][j];
}

void FrameBuffer::Clear(BUFFER_TYPE t)
{
	switch (t) {
	case BUFFER_TYPE::DEPTH_BUFFER:
		for (int i = 0; i < m_Width; ++i) {
			for (int j = 0; j < m_Height; ++j)
				m_DepthBuffer[i][j] = m_FillDepth;
		}
		break;
	case BUFFER_TYPE::COLOR_BUFFER:
		for (int i = 0; i < m_Width; ++i) {
			for (int j = 0; j < m_Height; ++j)
				m_ColorBuffer.set(i, j, m_FillColor);
		}
		break;
	}
}

int FrameBuffer::getWidth() const
{
	return m_Width;
}

int FrameBuffer::getHeight() const
{
	return m_Height;
}