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

	std::vector<glm::vec4> vertices;
	std::vector<glm::vec3> textureCoords;
	std::vector<glm::vec3> normals;
	std::vector<Face> faces;

	Mesh(std::string location);
};