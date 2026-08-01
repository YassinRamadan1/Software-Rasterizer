#include "texture.h"

Texture::Texture(std::string texturePath)
{
	m_Texture.resize(1);
	m_Texture[0].read_tga_file(texturePath);
}

Texture::Texture(TGAImage texture)
{
	m_Texture.emplace_back(std::move(texture));
}

glm::vec3 Texture::getTexel(float u, float v, float level) const
{
	switch (m_WrapModeU) {
	case WrapMode::REPEAT:
		u = repeat(u);
		break;
	case WrapMode::MIRROR:
		u = mirroredRepeat(u);
		break;
	case WrapMode::CLAMP:
		u = clamp(u);
		break;
	}

	switch (m_WrapModeV) {
	case WrapMode::REPEAT:
		v = repeat(v);
		break;
	case WrapMode::MIRROR:
		v = mirroredRepeat(v);
		break;
	case WrapMode::CLAMP:
		v = std::max(0.0f, std::min(1.0f, v));
		break;
	}

	level = std::clamp(level, 0.0f, float(m_Texture.size() - 1));

	switch (m_FilterMode) {
	case FilterMode::NEAREST:
		return nearestFilter(u, v, int(level + 0.5));
	case FilterMode::BILINEAR:
		return bilinearFilter(u, v, int(level + 0.5));
	case FilterMode::TRILINEAR:
		return trilinearFilter(u, v, level);
	}
}

float Texture::getMipmapLevel(glm::vec2 tex00, glm::vec2 tex10, glm::vec2 tex01) const
{
	tex10 -= tex00;
	tex01 -= tex00;

	tex10 *= glm::vec2(m_Texture[0].width(), m_Texture[0].height());
	tex01 *= glm::vec2(m_Texture[0].width(), m_Texture[0].height());

	float l = std::max(glm::length2(tex10), glm::length2(tex01));
	if (l <= 0.)
		return 0;

	l = std::log2(l) / 2.0;

	if (l >= m_Texture.size())
		return m_Texture.size() - 1;
	return l;
}

void Texture::generateMipmaps()
{
	int width = m_Texture[0].width();
	int height = m_Texture[0].height();
	m_Texture.reserve(15);
	glm::vec3 c, temp;
	for (int i = 1; true; ++i) {
		m_Texture.emplace_back((width + 1) / 2, (height + 1) / 2, TGAImage::RGB);
		for (int j = 0; j < width / 2; ++j)
			for (int k = 0; k < height / 2; ++k)
			{
				c = utility::toColor(m_Texture[i - 1].get(2 * j, 2 * k)) +
					utility::toColor(m_Texture[i - 1].get(2 * j + 1, 2 * k)) +
					utility::toColor(m_Texture[i - 1].get(2 * j, 2 * k + 1)) +
					utility::toColor(m_Texture[i - 1].get(2 * j + 1, 2 * k + 1));
				c *= 0.25;
				m_Texture[i].set(j, k, utility::toTGAColor(c));
			}

		if (width % 2)
		{
			for (int k = 0; k < height / 2; ++k)
			{
				c = utility::toColor(m_Texture[i - 1].get(width - 2, 2 * k)) +
					utility::toColor(m_Texture[i - 1].get(width - 1, 2 * k)) +
					utility::toColor(m_Texture[i - 1].get(width - 2, 2 * k + 1)) +
					utility::toColor(m_Texture[i - 1].get(width - 1, 2 * k + 1));
				c *= 0.25f;

				m_Texture[i].set(width / 2, k, utility::toTGAColor(c));
			}
		}

		if (height % 2)
		{
			for (int j = 0; j < width / 2; j++)
			{
				c = utility::toColor(m_Texture[i - 1].get(2 * j, height - 2)) +
					utility::toColor(m_Texture[i - 1].get(2 * j + 1, height - 2)) +
					utility::toColor(m_Texture[i - 1].get(2 * j, height - 1)) +
					utility::toColor(m_Texture[i - 1].get(2 * j + 1, height - 1));
				c *= 0.25f;

				m_Texture[i].set(j, height / 2, utility::toTGAColor(c));
			}
		}

		if (width % 2 && height % 2)
		{
			c = utility::toColor(m_Texture[i - 1].get(width - 2, height - 2)) +
				utility::toColor(m_Texture[i - 1].get(width - 1, height - 2)) +
				utility::toColor(m_Texture[i - 1].get(width - 2, height - 1)) +
				utility::toColor(m_Texture[i - 1].get(width - 1, height - 1));
			c *= 0.25f;

			m_Texture[i].set(width / 2, height / 2, utility::toTGAColor(c));
		}

		width = (width + 1) / 2, height = (height + 1) / 2;
		if (width == 1 && height == 1)
			break;
	}
}

void Texture::storeMipmaps(std::string mipmapName)
{
	for (int i = 0; i < m_Texture.size(); ++i)
	{
		m_Texture[i].write_tga_file(RESOURCES_PATH + mipmapName + std::to_string(i));
	}
}

glm::vec3 Texture::bilinearFilter(float u, float v, int level) const
{
	u *= m_Texture[level].width();
	v *= m_Texture[level].height();

	int x1 = u;
	int y1 = v;
	float fractionU = u - (x1 + 0.5);
	float fractionV = v - (y1 + 0.5);

	glm::vec3 color1(utility::toColor(m_Texture[level].get(x1, y1)));
	glm::vec3 color2(utility::toColor(m_Texture[level].get(x1 + 1, y1)));
	glm::vec3 color3(utility::toColor(m_Texture[level].get(x1, y1 + 1)));
	glm::vec3 color4(utility::toColor(m_Texture[level].get(x1 + 1, y1 + 1)));

	color2 = (1 - fractionU) * color1  + fractionU * color2;

	color4 = (1 - fractionU) * color3 + fractionU * color4;

	color4 = (1 - fractionV) * color2 + fractionV * color4;

	return color4;
}

glm::vec3 Texture::trilinearFilter(float u, float v, float level) const
{
	int downLevel = level;
	glm::vec3 color1 = bilinearFilter(u, v, downLevel), color2;

	if (downLevel + 1 >= m_Texture.size())
		return color1;

	color2 = bilinearFilter(u, v, downLevel + 1);
	float fractionW = level - downLevel;

	color2 = (1 - fractionW) * color1 + fractionW * color2;

	return color2;
}

glm::vec3 Texture::nearestFilter(float u, float v, int level) const
{
	u *= m_Texture[level].width();
	v *= m_Texture[level].height();

	return utility::toColor(m_Texture[level].get(int(u), int(v)));
}

float Texture::repeat(float u) const
{
	// we map 1.25 to 0.25 and we map integers to 0
	// we map -0.25 to 0.75 and -0.75 to 0.25 to not reflect the repeat
	return u - floor(u);
}

float Texture::mirroredRepeat(float u) const
{
	// we map 1.25 to 0.75 and we map integers to 0
	// we map -0.25 to 0.25 and -0.75 to 0.75 to reflect the repeat
	return 1 - repeat(u);
}

float Texture::clamp(float u) const
{
	return std::max(0.0f, std::min(1.0f, u));
}

int Texture::getTextureWidth(int level) const
{
	level = std::min(std::max(0, level), int(m_Texture.size() - 1));
	return m_Texture[level].width();
}

int Texture::getTextureHeight(int level) const
{
	level = std::min(std::max(0, level), int(m_Texture.size() - 1));
	return m_Texture[level].height();
}

void Texture::addTexture(std::string texturePath, int level)
{
	if (level > m_Texture.size())
	{
		std::cout << "un matching level\n";
		return;
	}
	TGAImage texture;
	texture.read_tga_file(texturePath);
	if (level == m_Texture.size())
		m_Texture.emplace_back(std::move(texture));
	m_Texture[level] = std::move(texture);
}

void Texture::addTexture(TGAImage texture, int level)
{
	if (level > m_Texture.size())
	{
		std::cout << "un matching level\n";
		return;
	}
	if (level == m_Texture.size())
		m_Texture.emplace_back(std::move(texture));
	m_Texture[level] = std::move(texture);
}

void Texture::updateTexture(std::string texturePath)
{
	m_Texture.resize(1);
	m_Texture[0].read_tga_file(texturePath);
}

void Texture::updateTexture(TGAImage texture)
{
	m_Texture.resize(1);
	m_Texture[0] = std::move(texture);
}

void Texture::setWrapModeU(WrapMode Wrapmode)
{
	m_WrapModeU = Wrapmode;
}

void Texture::setWrapModeV(WrapMode Wrapmode)
{
	m_WrapModeV = Wrapmode;
}

void Texture::setFilterMode(FilterMode Fitlermode)
{
	m_FilterMode = Fitlermode;
}