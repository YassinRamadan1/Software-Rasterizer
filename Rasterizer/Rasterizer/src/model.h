#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "tgaimage.h"
#include "utility.h"

class Model {

public:

	Model(std::string location) {
		
		std::ifstream file;
		file.open(location);
		if (file.fail()) {
			std::cout << "Failed to load the model with path: " << location << '\n';
			file.close();
			return;
		}
		std::string line, token;
		std::stringstream iss;
		glm::vec3 v;
		glm::vec3 tex;
		Face f;
		while (std::getline(file, line)) {
			
			if (line.empty())
				continue;

			std::vector<std::string> tokens;
			iss.clear();
			iss.str(line);
			while (std::getline(iss, token, ' '))
				tokens.push_back(token);
			
			
			if (tokens[0] == "v") {
				for (int i = 1; i < 4; i++)
					v[i - 1] = std::stof(tokens[i]);
				m_Vertices.push_back(v);
			}
			else if (tokens[0] == "f") {

				for (int i = 1; i < 4; i++) {
					iss.clear();
					iss.str(tokens[i]);
					std::getline(iss, token, '/');
					f.position[i - 1] = std::stoi(token) - 1;

					std::getline(iss, token, '/');
					f.texture[i - 1] = std::stoi(token) - 1;
				}
				m_Faces.push_back(f);
			}
			else if (tokens[0] == "vt") {
				tex[0] = std::stof(tokens[2]);
				tex[1] = std::stof(tokens[3]);
				tex[2] = std::stof(tokens[4]);
				m_Textures.push_back(tex);
			}
			else if (tokens[0] == "vn") {

			}
		}
		file.close();
	}

	std::vector<glm::vec3> m_Vertices;
	std::vector<glm::vec3> m_Textures;
	std::vector<Face> m_Faces;
};