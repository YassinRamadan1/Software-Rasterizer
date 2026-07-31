#include "vertexProcessor.h"

namespace vertex_processor
{
	std::vector<glm::vec4> transformedVertices;

	void processVertices(const std::vector<glm::vec4>& vertices, glm::mat4 mvp)
	{
		transformedVertices.clear();
		transformedVertices.reserve(vertices.size());
		for (int i = 0; i < vertices.size(); ++i)
		{
			transformedVertices.emplace_back(mvp * vertices[i]);
		}
	}
};