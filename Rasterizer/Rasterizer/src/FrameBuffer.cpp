#include "FrameBuffer.h"

FrameBuffer::FrameBuffer(int width, int height, int pixelFormat, int startX, int startY, float near, float far)
	: m_Width(width), m_Height(height), m_PixelFormat(pixelFormat), m_ColorBuffer(width, height, pixelFormat), m_X(startX), m_Y(startY), m_Near(near), m_Far(far) {

	m_DepthBuffer = new float* [m_Width];
	for (int i = 0; i < m_Width; i++) {
		m_DepthBuffer[i] = new float[m_Height];
		for (int j = 0; j < m_Height; j++)
			m_DepthBuffer[i][j] = m_Depth, m_ColorBuffer.set(i, j, m_Color.toTGAColor());
	}
	viewport = glm::mat4(m_Width / 2.0f, 0.0f, 0.0f, 0.0f,
		0.0f, m_Height / 2.0f, 0.0f, 0.0f,
		0.0f, 0.0f, (m_Far - m_Near) / 2.0f, 0.0f,
		m_X + m_Width / 2.0f, m_Y + m_Height / 2.0f, (m_Far + m_Near) / 2.0f, 1.0f);

}

bool FrameBuffer::read_Color_as_tga_file(const std::string filename) {

	return m_ColorBuffer.read_tga_file(filename);
}

bool FrameBuffer::write_Color_as_tga_file(const std::string filename, const bool vflip, const bool rle) {

	return m_ColorBuffer.write_tga_file(filename, vflip, rle);
}

bool FrameBuffer::write_Depth_as_tga_file(const std::string filename, const bool vflip, const bool rle) {

	TGAImage image(m_Width, m_Height, TGAImage::GRAYSCALE);

	for (int i = 0; i < m_Width; i++) {
		for (int j = 0; j < m_Height; j++)
			;// image.set(i, j, );
	}

	return image.write_tga_file(filename, vflip, rle);
}

void FrameBuffer::flip_Depth_horizontally() {

	for (int i = 0; i < m_Height; i++) {
		for (int j = 0; j < m_Width / 2; j++)
			std::swap(m_DepthBuffer[j][i], m_DepthBuffer[m_Width - j - 1][i]);
	}
}

void FrameBuffer::flip_Color_horizontally() {

	m_ColorBuffer.flip_horizontally();

	
}

void FrameBuffer::flip_Depth_vertically() {

	for (int i = 0; i < m_Width; i++) {
		for (int j = 0; j < m_Height / 2; j++)
			std::swap(m_DepthBuffer[i][j], m_DepthBuffer[i][m_Height - j - 1]);
	}
}

void FrameBuffer::flip_Color_vertically() {

	m_ColorBuffer.flip_vertically();
}

void FrameBuffer::setFillColor(Color color) {

	m_Color = color;
}

void FrameBuffer::setFillDepth(float depth) {

	m_Depth = depth;
}

void FrameBuffer::setPixelColor(int i, int j, Color color) {

	m_ColorBuffer.set(i, j, color.toTGAColor());
}

Color FrameBuffer::getPixelColor(int i, int j) {

	return Color(m_ColorBuffer.get(i, j));
}

void FrameBuffer::setPixelDepth(int i, int j, float depth) {

	m_DepthBuffer[i][j] = depth;
}

float FrameBuffer::getPixelDepth(int i, int j) {

	return m_DepthBuffer[i][j];
}

void FrameBuffer::Clear(BUFFER_TYPE t) {

	switch (t) {
	case BUFFER_TYPE::DEPTH_BUFFER:
		for (int i = 0; i < m_Width; i++) {
			for (int j = 0; j < m_Height; j++)
				m_DepthBuffer[i][j] = m_Depth;
		}
		break;
	case BUFFER_TYPE::COLOR_BUFFER:
		for (int i = 0; i < m_Width; i++) {
			for (int j = 0; j < m_Height; j++)
				m_ColorBuffer.set(i, j, m_Color.toTGAColor());
		}
		break;
	}
}

int FrameBuffer::width() {
	return m_Width;
}

int FrameBuffer::height() {
	return m_Height;
}

float FrameBuffer::near() {

	return m_Near;
}

float FrameBuffer::far() {

	return m_Far;
}

glm::mat4& FrameBuffer::getViewport() {

	return viewport;
}