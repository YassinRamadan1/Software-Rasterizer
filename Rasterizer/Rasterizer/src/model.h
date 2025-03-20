#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "tgaimage.h"
#include "draw.h"
#include "utility.h"

struct Face {
	
	int position[3];
	int texture[3];

	Face() {}

};

class Model {

public:

	Model(std::string location, TGAImage& framebuffer) : m_Framebuffer(framebuffer) {
		
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

	void draw(RenderingMode m, utility::Transform&transform, float* zBuffer, TGAImage& texture) {

		glm::vec3 world[3];
		glm::vec3 tex[3];
		glm::vec3 screen[3];
		glm::vec3 c1 = glm::vec3(255.0f, 0.0f, 0.0f);
		glm::vec3 c2 = glm::vec3(0.0f, 255.0f, 0.0f);
		glm::vec3 c3 = glm::vec3(0.0f, 0.0f, 255.0f);

		TGAColor c = { 0, 0, 255 };
		glm::vec3 lightDir(0, 0, -1);

		switch (m) {
			case WIREFRAME:
				for (int i = 0; i < m_Faces.size(); i++) {
					
					for (int j = 0; j < 3; j++) {
						world[j] = m_Vertices[m_Faces[i].position[j]];

					}

					drawTriangleWireFrame(screen[0], screen[1], screen[2], m_Framebuffer, c);
				}
			case SOLID:
				for (int i = 0; i < m_Faces.size(); i++) {
					
					for (int j = 0; j < 3; j++) {
						world[j] = m_Vertices[m_Faces[i].position[j]];
						tex[j] = m_Textures[m_Faces[i].texture[j]];
						
					}
					//rasterize(utility::Triangle(world[0], world[1], world[2], tex[0], tex[1], tex[2]), transform, texture, zBuffer, m_Framebuffer);
					rasterize(utility::Triangle(world[0], world[1], world[2], c1, c2, c3), transform, texture, zBuffer, m_Framebuffer);
				}
		}
	}
private:

	std::vector<glm::vec3> m_Vertices;
	std::vector<glm::vec3> m_Textures;
	std::vector<Face> m_Faces;
	TGAImage& m_Framebuffer;
};