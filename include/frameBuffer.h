#pragma once

#include "utility.h"

class FrameBuffer
{
private:
	int m_Width;
	int m_Height;
	float m_FillDepth;
	float** m_DepthBuffer;
	TGAColor m_FillColor;
	TGAImage m_ColorBuffer;

public:
	FrameBuffer(int width, int height, glm::vec3 fillColor = glm::vec3(0.0f), float fillDepth = std::numeric_limits<float>::max());

	bool loadColorBuffer(const std::string filename);
	
	bool storeColorBuffer(const std::string filename, const bool vflip = true, const bool rle = true);

	bool storeDepthBuffer(const std::string filename, const bool vflip = true, const bool rle = true);

	void flipHorizontally();

	void flipVertically();

	void Clear(BUFFER_TYPE type);

	void setFillColor(glm::vec3 color);

	void setFillDepth(float depth);

	void setPixelColor(int i, int j, glm::vec3 color);

	glm::vec3 getPixelColor(int i, int j) const;

	void setPixelDepth(int i, int j, float depth);

	float getPixelDepth(int i, int j) const;

	int getWidth() const;

	int getHeight() const;
};