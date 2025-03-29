#include "utility.h"

namespace utility {

	glm::mat4 perspectiveProjection(float fovy, float aspectRatio, float near, float far) {

		float t = near * glm::tan(fovy / 2.0f), r = t * aspectRatio;

		return glm::perspective(fovy, aspectRatio, near, far);
	}

	glm::mat4 orthographicProjection(float fovy, float aspectRatio, float near, float far) {

		float t = near * glm::tan(fovy / 2.0f), r = t * aspectRatio;

		return glm::ortho(-r, r, -t, t, near, far);
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
}