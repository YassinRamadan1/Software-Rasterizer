#pragma once

#include "utility.h"

class PointsProcessor {

	std::vector<glm::vec4>* m_Vertices;
	std::vector<glm::vec4> m_TransformedVertices;
	glm::mat4* m_Model, *m_View, *m_Projection;
public:
	
	PointsProcessor();

	void setModelMatrix(glm::mat4& model);

	void setViewMatrix(glm::mat4& view);

	void setProjectionMatrix(glm::mat4& projection);

	void setVertices(std::vector<glm::vec4>& vertices);

	void processPoints();

	std::vector<glm::vec4>& getTransformedVertices();
};