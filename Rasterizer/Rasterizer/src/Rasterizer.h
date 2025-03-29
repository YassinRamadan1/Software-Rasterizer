#pragma once

#include "Color.h"
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
	std::vector<Color> m_Colors;
	std::vector<Face> m_Faces;
	glm::mat4 m_Model, m_View, m_Projection;
	RenderMode m_RenderMode;
	ProjectionMode m_ProjectionMode; 
	AttributeMode m_AttributeMode;
	WrapMode m_WrapModeU, m_WrapModeV;
	FilterMode m_FilterMode;
	TGAImage* m_Texture;
	FrameBuffer* m_Framebuffer;
	float m_Far, m_Near;

public:

	Rasterizer(std::vector<glm::vec4> vertices, std::vector<Face> faces, glm::mat4 model, glm::mat4 view, glm::mat4 projection,
		FrameBuffer& framebuffer, TGAImage& texture, RenderMode renderMode = RenderMode::SOLID,
		ProjectionMode projectionMode = ProjectionMode::PERSPECTIVE, AttributeMode attributeMode = AttributeMode::TEXTURE,
		WrapMode wrapModeU = WrapMode::REPEAT, WrapMode wrapModeV = WrapMode::REPEAT,
		FilterMode filterMode = FilterMode::NEAREST, float near = 1.0f, float far = 100.0f);

	void process();

	void draw();

	void setVertices(std::vector<glm::vec4> vertices);

	void setTextureCoords(std::vector<glm::vec3> textureCoords);

	void setColors(std::vector<Color> colors);

	void setFaces(std::vector<Face> faces);

	void setModelMatrix(glm::mat4 model);

	void setViewMatrix(glm::mat4 view);

	void setProjectionMatrix(glm::mat4 projection);

	void setRenderMode(RenderMode renderMode);

	void setProjectionMode(ProjectionMode projectionMode);

	void setAttributeMode(AttributeMode attributeMode);

	void setWrapModeU(WrapMode wrapModeU);

	void setWrapModeV(WrapMode wrapModeV);

	void setFilterMode(FilterMode filterMode);

	void setFramebuffer(FrameBuffer& framebuffer);

	void setTexture(TGAImage& texture);

	void setFar(float far);

	void setNear(float near);
};