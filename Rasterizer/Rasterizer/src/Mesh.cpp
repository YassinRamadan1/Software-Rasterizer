#include "Mesh.h"

Mesh::Mesh(std::string location) {

	std::ifstream file;
	file.open(location);
	if (file.fail()) {
		std::cout << "Failed to load the model with path: " << location << '\n';
		file.close();
		return;
	}

	std::string line, token;
	glm::vec4 element;
	Face f;

	while (std::getline(file, line)) {

		if (line.empty())
			continue;

		// split the string according to ' ' and exclude empty strings
		std::vector<std::string> tokens = utility::split(line, ' ');

		if (tokens[0] == "v") {

			element[0] = element[1] = element[2] = 0.0f, element[3] = 1.0f;

			element[0] = std::stof(tokens[1]);
			element[1] = std::stof(tokens[2]);
			element[2] = std::stof(tokens[3]);
			m_Vertices.push_back(element);
		}
		else if (tokens[0] == "f") {

			std::vector<std::string> temp;
			for (int i = 1; i < 4; i++) {

				temp = utility::split(tokens[i], '/');
				f.position[i - 1] = std::stoi(temp[0]) - 1;
				f.texture[i - 1] = std::stoi(temp[1]) - 1;
				f.normal[i - 1] = std::stoi(temp[2]) - 1;
			}
			m_Faces.push_back(f);
		}
		else if (tokens[0] == "vt") {

			element[0] = element[1] = element[2] = 0.0f;

			// to ensure it doesn't go out of bound if the texture has 2 coordinates only
			for (int i = 1; i < tokens.size(); i++)
				element[i - 1] = std::stof(tokens[i]);
			m_Textures.push_back(element);
		}
		else if (tokens[0] == "vn") {

			element[0] = std::stof(tokens[1]);
			element[1] = std::stof(tokens[2]);
			element[2] = std::stof(tokens[3]);
			m_Normals.push_back(element);
		}
	}
	file.close();
}