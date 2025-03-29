#include "PointsProcessor.h"

PointsProcessor::PointsProcessor() : m_Vertices(nullptr), m_Model(nullptr), m_View(nullptr), m_Projection(nullptr) {}

void PointsProcessor::processPoints() {

	glm::mat4 mvp = (*m_Projection) * (*m_View) * (*m_Model);
	m_TransformedVertices.clear();
	m_TransformedVertices.resize(m_Vertices->size());
	for (int i = 0; i < m_Vertices->size(); i++) {
		m_TransformedVertices[i] = mvp * (*m_Vertices)[i];
	}
}

std::vector<glm::vec4>& PointsProcessor::getTransformedVertices() {
	return m_TransformedVertices;
}

void PointsProcessor::setModelMatrix(glm::mat4& model) {
	m_Model = &model;
}

void PointsProcessor::setViewMatrix(glm::mat4& view) {
	m_View = &view;
}

void PointsProcessor::setProjectionMatrix(glm::mat4& projection) {
	m_Projection = &projection;
}

void PointsProcessor::setVertices(std::vector<glm::vec4>& vertices) {
	m_Vertices = &vertices;
}