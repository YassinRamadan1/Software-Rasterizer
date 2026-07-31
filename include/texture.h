#pragma once
#include "utility.h"

class Texture
{
private:

	std::vector<TGAImage> m_Texture;
	FilterMode m_FilterMode;
	WrapMode m_WrapModeU;
	WrapMode m_WrapModeV;

public:
	Texture(std::string texturePath);

	Texture(TGAImage texture);

	glm::vec3 getTexel(float u, float v, float level) const;

	void generateMipmaps();

	float Texture::getMipmapLevel(glm::vec2 tex00, glm::vec2 tex10, glm::vec2 tex01) const;
	
	void storeMipmaps(std::string mipmapName);
	
	int getTextureWidth(int level) const;

	int getTextureHeight(int level) const;

	void Texture::addTexture(std::string texturePath, int level);

	void Texture::addTexture(TGAImage texture, int level);

	void updateTexture(std::string texturePath);

	void updateTexture(TGAImage texture);

	void setWrapModeU(WrapMode Wrapmode);

	void setWrapModeV(WrapMode Wrapmode);

	void setFilterMode(FilterMode Fitlermode);

private:
	glm::vec3 bilinearFilter(float u, float v, int level) const;

	glm::vec3 trilinearFilter(float u, float v, float level) const;

	glm::vec3 nearestFilter(float u, float v, int level) const;

	float repeat(float u) const;

	float mirroredRepeat(float u) const;

	float clamp(float u) const;
};