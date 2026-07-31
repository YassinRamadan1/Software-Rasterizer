#pragma once
#include "utility.h"

namespace vertex_processor
{
	extern std::vector<glm::vec4> transformedVertices;

	void processVertices(const std::vector<glm::vec4>& vertices, glm::mat4 mvp);
};