#include "FrameBuffer.h"

FrameBuffer::FrameBuffer(int width, int height, NumberOfSamples numberOfSamples, glm::vec3 fillColor, float fillDepth)
	: m_Width(width), m_Height(height), m_FillDepth(fillDepth), m_FillColor(fillColor)
{
	m_NumberOfSamples = static_cast<int>(numberOfSamples);
	m_DepthBuffer = new float** [m_Width];
	m_ColorBuffer = new glm::vec3** [m_Width];

	for (int i = 0; i < m_Width; ++i)
	{
		m_DepthBuffer[i] = new float* [m_Height];
		m_ColorBuffer[i] = new glm::vec3* [m_Height];
		for (int j = 0; j < m_Height; ++j)
		{
			m_DepthBuffer[i][j] = new float [m_NumberOfSamples];
			m_ColorBuffer[i][j] = new glm::vec3 [m_NumberOfSamples];
			for (int k = 0; k < m_NumberOfSamples; ++k)
			{
				m_DepthBuffer[i][j][k] = m_FillDepth;
				m_ColorBuffer[i][j][k] = m_FillColor;
			}
		}
	}
}

bool FrameBuffer::storeColorBuffer(const std::string filename, const bool vflip, const bool rle)
{
	TGAImage temp(m_Width, m_Height, 4);
	for (int i = 0; i < m_Width; ++i)
	{
		for (int j = 0; j < m_Height; ++j)
		{
			glm::vec3 totalSum(0.);
			for (int k = 0; k < m_NumberOfSamples; ++k)
			{
				totalSum += m_ColorBuffer[i][j][k];
			}
			totalSum /= m_NumberOfSamples;
			temp.set(i, j, clr::toTGAColor(clr::gammaCorrect(totalSum, 2.2)));
		}
	}
	return temp.write_tga_file(filename, vflip, rle);
}

void FrameBuffer::flipHorizontally()
{	
	for (int i = 0; i < m_Height; ++i)
	{
		for (int j = 0; j < m_Width / 2; ++j)
		{
			for (int k = 0; k < m_NumberOfSamples; ++k)
			{
				std::swap(m_DepthBuffer[j][i][k], m_DepthBuffer[m_Width - j - 1][i][k]);
				std::swap(m_ColorBuffer[j][i][k], m_ColorBuffer[m_Width - j - 1][i][k]);
			}
		}
	}
}

void FrameBuffer::flipVertically()
{
	for (int i = 0; i < m_Width; ++i)
	{
		for (int j = 0; j < m_Height / 2; ++j)
		{
			for (int k = 0; k < m_NumberOfSamples; ++k)
			{
				std::swap(m_DepthBuffer[i][j][k], m_DepthBuffer[i][m_Height - j - 1][k]);
				std::swap(m_ColorBuffer[i][j][k], m_ColorBuffer[i][m_Height - j - 1][k]);
			}
		}
	}
}

void FrameBuffer::setFillColor(glm::vec3 color)
{
	m_FillColor = color;
}

void FrameBuffer::setFillDepth(float depth)
{
	m_FillDepth = depth;
}

void FrameBuffer::setSampleColor(int i, int j, int k, glm::vec3 color)
{
	m_ColorBuffer[i][j][k] = color;
}

glm::vec3 FrameBuffer::getSampleColor(int i, int j, int k) const
{
	return m_ColorBuffer[i][j][k];
}

void FrameBuffer::setSampleDepth(int i, int j, int k, float depth)
{
	m_DepthBuffer[i][j][k] = depth;
}

float FrameBuffer::getSampleDepth(int i, int j, int k) const
{
	return m_DepthBuffer[i][j][k];
}

void FrameBuffer::clear(BufferType type)
{
	switch (type)
	{
	case BufferType::DEPTH_BUFFER:
		for (int i = 0; i < m_Width; ++i)
		{
			for (int j = 0; j < m_Height; ++j)
			{
				for (int k = 0; k < m_NumberOfSamples; ++k)
				{
					m_DepthBuffer[i][j][k] = m_FillDepth;
				}
			}
		}
		break;
	case BufferType::COLOR_BUFFER:
		for (int i = 0; i < m_Width; ++i)
		{
			for (int j = 0; j < m_Height; ++j)
			{
				for (int k = 0; k < m_NumberOfSamples; ++k)
				{
					m_ColorBuffer[i][j][k] = m_FillColor;
				}
			}
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

int FrameBuffer::getNumberOfSamples() const
{
	return m_NumberOfSamples;
}