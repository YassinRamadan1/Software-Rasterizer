#pragma once

#include "PointsProcessor.h"
#include "TextureProcessor.h"
#include "PrimitiveAssembler.h"
#include "Drawer.h"

class Rasterizer {

private:

	PointsProcessor m_PointsProcessor;
	TextureProcessor m_TextureProcessor;
	PrimitiveAssembler m_PrimitiveAssembler;
	Drawer m_Drawer;

	std::vector<glm::vec4> m_Vertices;
	std::vector<glm::vec3> m_TextureCoords;
	std::vector<utility::Color> m_Colors;
	std::vector<Face> m_Faces;
	glm::mat4 m_Model, m_View, m_Projection, m_Viewport;
	RenderMode m_RenderMode;
	ProjectionMode m_ProjectionMode; 
	AttributeMode m_AttributeMode;
	WrapMode m_WrapModeU, m_WrapModeV;
	FilterMode m_FilterMode;
	TGAImage* m_Framebuffer, * m_Texture;
	float* m_DepthBuffer, m_Far, m_Near, m_Viewport_Near = 0, m_Viewport_Far = 1;

public:

	Rasterizer(std::vector<glm::vec4> vertices, std::vector<Face> faces, glm::mat4 model, glm::mat4 view, glm::mat4 projection,
		glm::mat4 viewport, TGAImage& framebuffer, TGAImage& texture, float* depthBuffer, RenderMode renderMode = RenderMode::SOLID,
		ProjectionMode projectionMode = ProjectionMode::PERSPECTIVE, AttributeMode attributeMode = AttributeMode::TEXTURE, WrapMode wrapModeU = WrapMode::REPEAT,
		WrapMode wrapModeV = WrapMode::REPEAT, FilterMode filterMode = FilterMode::NEAREST, float near = 1.0f, float far = 100.0f, float viewportNear = 0.0f,
		float viewportFar = 1.0f) 
		: m_Vertices(vertices), m_Faces(faces), m_Model(model), m_View(view), m_Projection(projection),
		m_Viewport(viewport), m_RenderMode(renderMode), m_ProjectionMode(projectionMode), m_AttributeMode(attributeMode),
		m_WrapModeU(wrapModeU), m_WrapModeV(wrapModeV), m_FilterMode(filterMode), m_Framebuffer(&framebuffer),
		m_Texture(&texture), m_DepthBuffer(depthBuffer), m_Near(near), m_Far(far), m_Viewport_Near(viewportNear),
		m_Viewport_Far(viewportFar) {}

	void process() {

		m_PointsProcessor.setModelMatrix(m_Model);
		m_PointsProcessor.setViewMatrix(m_View);
		m_PointsProcessor.setProjectionMatrix(m_Projection);
		m_PointsProcessor.setVertices(m_Vertices);

		m_TextureProcessor.setFilterMode(m_FilterMode);
		m_TextureProcessor.setWrapModeU(m_WrapModeU);
		m_TextureProcessor.setWrapModeV(m_WrapModeV);

		m_PrimitiveAssembler.setViewport(m_Viewport);
		m_PrimitiveAssembler.setFaces(m_Faces);

		switch (m_AttributeMode) {

		case AttributeMode::COLOR:
			m_PrimitiveAssembler.setColors(m_Colors);
			break;
		case AttributeMode::TEXTURE:
			m_PrimitiveAssembler.setTextureCoordinates(m_TextureCoords);
			m_TextureProcessor.setTexture(*m_Texture);
			m_TextureProcessor.generateMipmaps();
			break;
		}

		m_Drawer.setTextureProcessor(m_TextureProcessor);
		m_Drawer.setFramebuffer(m_Framebuffer);
		m_Drawer.setDepthBuffer(m_DepthBuffer);
		m_Drawer.setRenderMode(m_RenderMode);
		m_Drawer.setProjectionMode(m_ProjectionMode);
		m_Drawer.setAttributeMode(m_AttributeMode);
		m_Drawer.setFar(m_Far);
		m_Drawer.setNear(m_Near);
		m_Drawer.setViewportNear(m_Viewport_Near);
		m_Drawer.setViewportFar(m_Viewport_Far);
	}

	void draw() {

		m_PointsProcessor.processPoints();

		m_PrimitiveAssembler.setVertices(m_PointsProcessor.getTransformedVertices());
		
		m_PrimitiveAssembler.processPrimitives();

		m_Drawer.setTriangles(m_PrimitiveAssembler.getTriangles());
	
		m_Drawer.draw();
	}


	// Setters for vectors
	void setVertices(std::vector<glm::vec4> vertices) {
		m_Vertices = vertices;
	}

	void setTextureCoords(std::vector<glm::vec3> textureCoords) {
		m_TextureCoords = textureCoords;
	}

	void setColors(std::vector<utility::Color> colors) {
		m_Colors = colors;
	}

	void setFaces(std::vector<Face> faces) {
		m_Faces = faces;
	}

	// Setters for matrices
	void setModelMatrix(glm::mat4 model) {
		m_Model = model;
	}

	void setViewMatrix(glm::mat4 view) {
		m_View = view;
	}

	void setProjectionMatrix(glm::mat4 projection) {
		m_Projection = projection;
	}

	void setViewportMatrix(glm::mat4 viewport) {
		m_Viewport = viewport;
	}

	// Setters for enum types
	void setRenderMode(RenderMode renderMode) {
		m_RenderMode = renderMode;
	}

	void setProjectionMode(ProjectionMode projectionMode) {
		m_ProjectionMode = projectionMode;
	}

	void setAttributeMode(AttributeMode attributeMode) {
		m_AttributeMode = attributeMode;
	}

	void setWrapModeU(WrapMode wrapModeU) {
		m_WrapModeU = wrapModeU;
	}

	void setWrapModeV(WrapMode wrapModeV) {
		m_WrapModeV = wrapModeV;
	}

	void setFilterMode(FilterMode filterMode) {
		m_FilterMode = filterMode;
	}

	// Setters for pointers
	void setFramebuffer(TGAImage& framebuffer) {
		m_Framebuffer = &framebuffer;
	}

	void setTexture(TGAImage& texture) {
		m_Texture = &texture;
	}

	void setDepthBuffer(float* depthBuffer) {
		m_DepthBuffer = depthBuffer;
	}

	// Setters for float values
	void setFar(float far) {
		m_Far = far;
	}

	void setNear(float near) {
		m_Near = near;
	}

	void setViewportNear(float viewportNear) {
		m_Viewport_Near = viewportNear;
	}

	void setViewportFar(float viewportFar) {
		m_Viewport_Far = viewportFar;
	}
};