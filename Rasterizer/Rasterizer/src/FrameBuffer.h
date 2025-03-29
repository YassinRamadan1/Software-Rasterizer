#pragma once

#include "utility.h"
#include "Color.h"

class FrameBuffer {

private:

	int m_Width, m_Height, m_PixelFormat, m_X, m_Y;
	float m_Depth = std::numeric_limits<float>::max(), ** m_DepthBuffer, m_Near, m_Far;
	Color m_Color = Color();
	TGAImage m_ColorBuffer;
	glm::mat4 viewport;

public:

	FrameBuffer(int width, int height, int pixelFormat, int startX = 0, int startY = 0, float near = 0.0f, float far = 1.0f);

	bool read_Color_as_tga_file(const std::string filename);
	
	bool write_Color_as_tga_file(const std::string filename, const bool vflip = true, const bool rle = true);

	bool write_Depth_as_tga_file(const std::string filename, const bool vflip = true, const bool rle = true);

	void flip_Depth_horizontally();

	void flip_Color_horizontally();

	void flip_Depth_vertically();

	void flip_Color_vertically();

	void setFillColor(Color color);

	void setFillDepth(float depth);

	void setPixelColor(int i, int j, Color color);

	Color getPixelColor(int i, int j);

	void setPixelDepth(int i, int j, float depth);

	float getPixelDepth(int i, int j);

	void Clear(BUFFER_TYPE t);

	int width();

	int height();

	float near();

	float far();

	glm::mat4& getViewport();
};