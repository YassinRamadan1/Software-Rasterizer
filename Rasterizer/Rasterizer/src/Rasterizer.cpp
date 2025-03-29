#include "Rasterizer.h"

Rasterizer::Rasterizer(std::vector<glm::vec4> vertices, std::vector<Face> faces, glm::mat4 model, glm::mat4 view, glm::mat4 projection, 
	FrameBuffer& framebuffer, TGAImage& texture, RenderMode renderMode, ProjectionMode projectionMode,
	AttributeMode attributeMode, WrapMode wrapModeU, WrapMode wrapModeV, FilterMode filterMode, float near, float far)
	: m_Vertices(vertices), m_Faces(faces), m_Model(model), m_View(view), m_Projection(projection), m_RenderMode(renderMode),
	m_ProjectionMode(projectionMode), m_AttributeMode(attributeMode), m_WrapModeU(wrapModeU), m_WrapModeV(wrapModeV),
	m_FilterMode(filterMode), m_Framebuffer(&framebuffer), m_Texture(&texture), m_Near(near), m_Far(far) {}

void Rasterizer::process() {

	m_PointsProcessor.setModelMatrix(m_Model);
	m_PointsProcessor.setViewMatrix(m_View);
	m_PointsProcessor.setProjectionMatrix(m_Projection);
	m_PointsProcessor.setVertices(m_Vertices);

	m_TextureProcessor.setFilterMode(m_FilterMode);
	m_TextureProcessor.setWrapModeU(m_WrapModeU);
	m_TextureProcessor.setWrapModeV(m_WrapModeV);

	m_PrimitiveAssembler.setViewport(m_Framebuffer->getViewport());
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
	m_Drawer.setRenderMode(m_RenderMode);
	m_Drawer.setProjectionMode(m_ProjectionMode);
	m_Drawer.setAttributeMode(m_AttributeMode);
	m_Drawer.setFar(m_Far);
	m_Drawer.setNear(m_Near);
}

void Rasterizer::draw() {

	m_PointsProcessor.processPoints();

	m_PrimitiveAssembler.setVertices(m_PointsProcessor.getTransformedVertices());

	m_PrimitiveAssembler.processPrimitives();

	m_Drawer.setTriangles(m_PrimitiveAssembler.getTriangles());

	m_Drawer.draw();
}

void Rasterizer::setVertices(std::vector<glm::vec4> vertices) {
	m_Vertices = vertices;
}

void Rasterizer::setTextureCoords(std::vector<glm::vec3> textureCoords) {
	m_TextureCoords = textureCoords;
}

void Rasterizer::setColors(std::vector<Color> colors) {
	m_Colors = colors;
}

void Rasterizer::setFaces(std::vector<Face> faces) {
	m_Faces = faces;
}

void Rasterizer::setModelMatrix(glm::mat4 model) {
	m_Model = model;
}

void Rasterizer::setViewMatrix(glm::mat4 view) {
	m_View = view;
}

void Rasterizer::setProjectionMatrix(glm::mat4 projection) {
	m_Projection = projection;
}

void Rasterizer::setRenderMode(RenderMode renderMode) {
	m_RenderMode = renderMode;
}

void Rasterizer::setProjectionMode(ProjectionMode projectionMode) {
	m_ProjectionMode = projectionMode;
}

void Rasterizer::setAttributeMode(AttributeMode attributeMode) {
	m_AttributeMode = attributeMode;
}

void Rasterizer::setWrapModeU(WrapMode wrapModeU) {
	m_WrapModeU = wrapModeU;
}

void Rasterizer::setWrapModeV(WrapMode wrapModeV) {
	m_WrapModeV = wrapModeV;
}

void Rasterizer::setFilterMode(FilterMode filterMode) {
	m_FilterMode = filterMode;
}

void Rasterizer::setFramebuffer(FrameBuffer& framebuffer) {
	m_Framebuffer = &framebuffer;
}

void Rasterizer::setTexture(TGAImage& texture) {
	m_Texture = &texture;
}

void Rasterizer::setFar(float far) {
	m_Far = far;
}

void Rasterizer::setNear(float near) {
	m_Near = near;
}