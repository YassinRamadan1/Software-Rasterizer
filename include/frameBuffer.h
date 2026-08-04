#pragma once

#include "utility.h"

class FrameBuffer
{
private:
	int m_NumberOfSamples;
	int m_Width;
	int m_Height;
	float m_FillDepth;
	float*** m_DepthBuffer;
	glm::vec3 m_FillColor;
	glm::vec3*** m_ColorBuffer;

public:
	FrameBuffer(int width, int height, NumberOfSamples numberOfSamples = NumberOfSamples::ONE_SAMPLE, glm::vec3 fillColor = glm::vec3(0.0f), float fillDepth = std::numeric_limits<float>::max());
	
	bool storeColorBuffer(const std::string filename, const bool vflip = true, const bool rle = true);

	void flipHorizontally();

	void flipVertically();

	void clear(BufferType type);

	void setFillColor(glm::vec3 color);

	void setFillDepth(float depth);

	void setSampleColor(int i, int j, int k, glm::vec3 color);

	glm::vec3 getSampleColor(int i, int j, int k) const;

	void setSampleDepth(int i, int j, int k, float depth);

	float getSampleDepth(int i, int j, int k) const;

	int getWidth() const;

	int getHeight() const;

	int getNumberOfSamples() const;
};