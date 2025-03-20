#include "utility.h"

namespace utility {

	float cross(glm::vec2 v1, glm::vec2 v2, glm::vec2 v3) {
				return (v2.x - v1.x) * (v3.y - v1.y) - (v2.y - v1.y) * (v3.x - v1.x);
	}

	glm::mat4 viewport(int x, int y, int w, int h, int near, int far) {

		return glm::mat4(w / 2.0f, 0.0f, 0.0f, 0.0f,
						 0.0f, h / 2.0f, 0.0f, 0.0f,
						 0.0f, 0.0f, (far - near) / 2.0f, 0.0f,
						 x + w / 2.0f, y + h / 2.0f, (far + near) / 2.0f, 1.0f);
	}

	glm::mat4 perspectiveProjection(float fovy, float aspectRatio, float near, float far) {
	
		float t = near * glm::tan(fovy / 2.0f), r = t * aspectRatio;

		return glm::perspective(fovy, aspectRatio, near, far); /*glm::mat4(near / r, 0.0f, 0.0f, 0.0f,
						 0.0f, near / t, 0.0f, 0.0f,
						 0.0f, 0.0f, -(far + near) / (far - near), -1.0f,
						 0.0f, 0.0f, -(2.0f * far * near) / (far - near), 0.0f
						);*/
	}
	glm::mat4 orthographicProjection(float fovy, float aspectRatio, float near, float far) {

		float t = near * glm::tan(fovy / 2.0f), r = t * aspectRatio;

		return glm::ortho(-r, r, -t, t, near, far);
	}
}

