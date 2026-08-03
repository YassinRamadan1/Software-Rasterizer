#include "utility.h"

namespace utility
{
	glm::mat4 perspectiveProjection(float fovy, float aspectRatio, float near, float far)
	{
		return glm::perspective(fovy, aspectRatio, near, far);
	}

	glm::mat4 orthographicProjection(float fovy, float aspectRatio, float near, float far) {

		float t = near * glm::tan(fovy / 2.0f), r = t * aspectRatio;

		return glm::ortho(-r, r, -t, t, near, far);
	}

	glm::mat4 viewport(float x, float y, float width, float height, float near, float far)
	{
		glm::mat4 viewport = glm::mat4(width / 2.0f, 0.0f, 0.0f, 0.0f,
							0.0f, height / 2.0f, 0.0f, 0.0f,
							0.0f, 0.0f, (far - near) / 2.0f, 0.0f,
							x + width / 2.0f, y + height / 2.0f, (far + near) / 2.0f, 1.0f);
		return viewport;
	}

	std::vector<std::string> split(std::string& line, char delimiter) {

		std::string temp;
		std::vector<std::string> result;

		// To push the last element before the string ends
		line.push_back(delimiter);
		for (int i = 0; i < line.size(); i++) {

			if (line[i] == delimiter) {

				if (!temp.empty())
					result.push_back(temp);
				temp = "";
				continue;
			}
			temp.push_back(line[i]);
		}
		return result;
	}

	const glm::vec2* getMSAASamples(NumberOfSamples numberOfSamples)
	{
		switch (numberOfSamples)
		{
		case NumberOfSamples::ONE_SAMPLE:
			return MSAA1;
		case NumberOfSamples::TWO_SAMPLES:
			return MSAA2;
		case NumberOfSamples::FOUR_SAMPLES:
			return MSAA4;
		case NumberOfSamples::EIGHT_SAMPLES:
			return MSAA8;
		case NumberOfSamples::SIXTEEN_SAMPLES:
			return MSAA16;
		}
		return nullptr;
	}
}

namespace clr
{
	glm::vec3 toColor(TGAColor tgaColor)
	{
		glm::vec3 color;
		color.b = tgaColor[0] / 255.0f;
		color.g = tgaColor[1] / 255.0f;
		color.r = tgaColor[2] / 255.0f;
		return color;
	}

	TGAColor toTGAColor(glm::vec3 color)
	{
		TGAColor tgaColor;
		tgaColor[0] = static_cast<uint8_t>(std::clamp(color.b * 255.0f, 0.0f, 255.0f));
		tgaColor[1] = static_cast<uint8_t>(std::clamp(color.g * 255.0f, 0.0f, 255.0f));
		tgaColor[2] = static_cast<uint8_t>(std::clamp(color.r * 255.0f, 0.0f, 255.0f));
		tgaColor[3] = 255;
		return tgaColor;
	}

	glm::vec3 gammaUncorrect(glm::vec3 color, float gamma)
	{
		return glm::pow(color, glm::vec3(gamma));
	}

	glm::vec3 gammaCorrect(glm::vec3 color, float gamma)
	{
		return glm::pow(color, glm::vec3(1 / gamma));
	}
}