#pragma once

#include "utility.h"
class TextureProcessor {
private:

	std::vector<TGAImage> m_Texture;
	FilterMode m_FilterMode;
	WrapMode m_WrapModeU;
	WrapMode m_WrapModeV;

public:

	TextureProcessor();

	Color getTexel(float u, float v, float level);

	void generateMipmaps();

private:

	Color bilinearFilter(float u, float v, int level);

	Color trilinearFilter(float u, float v, float level);

	Color nearestFilter(float u, float v, int level);

	float Repeat(float u);

	float mirroredRepeat(float u);

	float clamp(float u);

public:

	int getTextureWidth(int level);

	int getTextureHeight(int level);

	void setTexture(TGAImage& texture);

	void setWrapModeU(WrapMode Wrapmode);

	void setWrapModeV(WrapMode Wrapmode);

	void setFilterMode(FilterMode Fitlermode);
};