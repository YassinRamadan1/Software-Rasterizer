#pragma once

#include "utility.h"
class TextureProcessor {
private:

	std::vector<TGAImage> m_Texture;
	FilterMode m_FilterMode;
	WrapMode m_WrapModeU;
	WrapMode m_WrapModeV;

public:

	TextureProcessor() {
	
		m_Texture.resize(1);
	}

	TGAColor getTexel(float u, float v, float level) {
		
		// wrapping method
		switch (m_WrapModeU) {
		case REPEAT:
			u = Repeat(u);
			break;
		case MIRROR:
			u = mirroredRepeat(u);
			break;
		case CLAMP:
			u = std::max(0.0f, std::min(1.0f, u));
			break;
		}

		switch (m_WrapModeV) {
		case REPEAT:
			v = Repeat(v);
			break;
		case MIRROR:
			v = mirroredRepeat(v);
			break;
		case CLAMP:
			v = std::max(0.0f, std::min(1.0f, v));
			break;
		}
		// clamping the level to the valid range
		level = std::clamp(level, 0.0f, float(m_Texture.size() - 1));
		// filtering method
		switch (m_FilterMode) {
			case NEAREST:
				return nearestFilter(u, v, int(level + 0.5));
			case BILINEAR:
				return bilinearFilter(u, v, int(level + 0.5));
			case TRILINEAR:
				return trilinearFilter(u, v, level);
		}
	}


	void generateMipmaps() {

		int width = m_Texture[0].width();
		int height = m_Texture[0].height();
		utility::Color c, temp;
		for (int i = 1; true; i++) {
			m_Texture.push_back(TGAImage((width + 1) / 2, (height + 1) / 2, TGAImage::RGB));
			for (int j = 0; j < width / 2; j++)
				for (int k = 0; k < height / 2; k++) {
					
					c.init(0);

					temp = temp.toColor(m_Texture[i - 1].get(2 * j, 2 * k));
					c = c + temp;

					temp = temp.toColor(m_Texture[i - 1].get(2 * j + 1, 2 * k));
					c = c + temp;

					temp = temp.toColor(m_Texture[i - 1].get(2 * j, 2 * k + 1));
					c = c + temp;

					temp = temp.toColor(m_Texture[i - 1].get(2 * j + 1, 2 * k + 1));
					c = c + temp;

					c = c * 0.25;

					m_Texture[i].set(j, k, c.toTGAColor());
				}

			if (width % 2) {
				for (int k = 0; k < height / 2; k++) {
					c.init(0);

					temp = temp.toColor(m_Texture[i - 1].get(width - 2, 2 * k));
					c = c + temp;

					temp = temp.toColor(m_Texture[i - 1].get(width - 1, 2 * k));
					c = c + temp;

					temp = temp.toColor(m_Texture[i - 1].get(width - 2, 2 * k + 1));
					c = c + temp;

					temp = temp.toColor(m_Texture[i - 1].get(width - 1, 2 * k + 1));
					c = c + temp;

					c = c * 0.25f;

					m_Texture[i].set(width / 2, k, c.toTGAColor());
				}
			}
			if (height % 2) {
				for (int j = 0; j < width / 2; j++) {
					c.init(0);

					temp = temp.toColor(m_Texture[i - 1].get(2 * j, height - 2));
					c = c + temp;
					temp = temp.toColor(m_Texture[i - 1].get(2 * j + 1, height - 2));
					c = c + temp;

					temp = temp.toColor(m_Texture[i - 1].get(2 * j, height - 1));
					c = c + temp;

					temp = temp.toColor(m_Texture[i - 1].get(2 * j + 1, height - 1));
					c = c + temp;

					c = c * 0.25f;
					m_Texture[i].set(j, height / 2, c.toTGAColor());
				}
			}

			if (width % 2 && height % 2) {
				c.init(0);
				temp = temp.toColor(m_Texture[i - 1].get(width - 2, height - 2));
				c = c + temp;

				temp = temp.toColor(m_Texture[i - 1].get(width - 1, height - 2));
				c = c + temp;

				temp = temp.toColor(m_Texture[i - 1].get(width - 2, height - 1));
				c = c + temp;

				temp = temp.toColor(m_Texture[i - 1].get(width - 1, height - 1));
				c = c + temp;

				c = c * 0.25f;

				m_Texture[i].set(width / 2, height / 2, c.toTGAColor());
			}

			width = (width + 1) / 2, height = (height + 1) / 2;
			if (width == 1 && height == 1)
				break;
		}
		for (int i = 0; i < m_Texture.size(); i++) {
			m_Texture[i].write_tga_file("texturelevel" + std::to_string(i) + ".tga");
		}
	}

private:

	TGAColor bilinearFilter(float u, float v, int level) {

		u = u * (m_Texture[level].width() - 1), v = v * (m_Texture[level].height() - 1);
		int x1 = floor(u), y1 = floor(v);
		float fractionU = u - x1;
		float fractionV = v - y1;

		// lerping down in the U direction 
		utility::Color color1 = color1.toColor(m_Texture[level].get(x1 + 1, y1)), color2 = color2.toColor(m_Texture[level].get(x1, y1)),
			color3;

		// color3 is the result of lerping down in the U direction
		color3 = color1 * fractionU + color2 * (1 - fractionU);

		// lerping up in the U direction 
		color1 = color1.toColor(m_Texture[level].get(x1 + 1, y1 + 1));
		color2 = color2.toColor(m_Texture[level].get(x1, y1 + 1));

		// color2 is the result of lerping up in the U direction
		color2 = color1 * fractionU + color2 * (1 - fractionU);

		// lerping in the V direction
		color1 = color2 * fractionV + color3 * (1 - fractionV);

		return color1.toTGAColor();
	}

	TGAColor trilinearFilter(float u, float v, float level) {

		utility::Color color1, color2;
		int downLevel = floor(level);
		color1 = color1.toColor(bilinearFilter(u, v, downLevel));
		if (downLevel + 1 >= m_Texture.size())
			return color1.toTGAColor();
		
		color2 = color2.toColor(bilinearFilter(u, v, downLevel + 1));
		float fractionW = level - downLevel;
		color1 = color2 * fractionW + color1 * (1 - fractionW);

		// color1 is the result of trilinear interpolation
		return color1.toTGAColor();
	}

	TGAColor nearestFilter(float u, float v, int level) {

		u *= (m_Texture[level].width() - 1), v *= (m_Texture[level].height() - 1);
		
		return m_Texture[level].get(int(u + 0.5), int(v + 0.5));
	}

	float Repeat(float u) {

		if (u > 1.0f || u < 0.0f) {
			u = u - floor(u);
			if (u < 1e-6)
				u = 1.0f;
		}
		return u;
	}

	float mirroredRepeat(float u) {

		if (u > 1.0f || u < 0.0f)
			u = 1 - Repeat(u);
		return u;
	}

public:

	int getTextureWidth(int level) {
		return m_Texture[level].width();
	}

	int getTextureHeight(int level) {
		return m_Texture[level].height();
	}

	void setTexture(TGAImage& texture) {
		m_Texture[0] = texture;
	}

	void setWrapModeU(WrapMode Wrapmode) {
		m_WrapModeU = Wrapmode;
	}

	void setWrapModeV(WrapMode Wrapmode) {
		m_WrapModeV = Wrapmode;
	}

	void setFilterMode(FilterMode Fitlermode) {
		m_FilterMode = Fitlermode;
	}

	
};