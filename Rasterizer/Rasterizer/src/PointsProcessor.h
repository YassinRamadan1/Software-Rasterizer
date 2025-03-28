#pragma once

#include "utility.h"

class PointsProcessor {

	std::vector<glm::vec4>* m_Vertices;
	std::vector<glm::vec4> m_TransformedVertices;
	glm::mat4* m_Model, *m_View, *m_Projection;
public:
	
	PointsProcessor() : m_Vertices(nullptr), m_Model(nullptr), m_View(nullptr), m_Projection(nullptr) {}

	void setModelMatrix(glm::mat4& model) {
		m_Model = &model;
	}

	void setViewMatrix(glm::mat4& view) {
		m_View = &view;
	}

	void setProjectionMatrix(glm::mat4& projection) {
		m_Projection = &projection;
	}

	void setVertices(std::vector<glm::vec4>& vertices) {
		m_Vertices = &vertices;
	}

	void processPoints() {
		
		glm::mat4 mvp = (*m_Projection) * (*m_View) * (*m_Model);
		glm::vec4 transformedVertex;
		for (auto& vertex : (*m_Vertices)) {
			transformedVertex = mvp * vertex;
			m_TransformedVertices.push_back(transformedVertex);
		}
	}

	std::vector<glm::vec4>& getTransformedVertices() {
		return m_TransformedVertices;
	}
};