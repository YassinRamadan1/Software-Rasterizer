#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "tgaimage.h"
#include "utility.h"

class Mesh {

public:

	std::vector<glm::vec4> m_Vertices;
	std::vector<glm::vec3> m_Textures;
	std::vector<glm::vec3> m_Normals;
	std::vector<Face> m_Faces;

	Mesh(std::string location);
};