#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "tgaimage.h"
#include "draw.h"
#include "geometry.h"

struct Face {
	
	int points[3];

	Face() {}

	Face(int p1, int p2, int p3) {
	
		points[0] = p1, points[0] = p2, points[0] = p3;
	}

};

class model {

public:

	model(std::string location, TGAImage& framebuffer) : m_Framebuffer(framebuffer) {
		
		std::ifstream file;
		file.open(location);
		if (file.fail()) {
			std::cout << "Failed to load the model with path: " << location << '\n';
			file.close();
			return;
		}
		std::string line, token;
		std::stringstream iss;
		vec3 v;
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
					v.raw[i - 1] = std::stof(tokens[i]);
				m_Vertices.push_back(v);
			}
			else if (tokens[0] == "f") {

				for (int i = 1; i < 4; i++) {
					iss.clear();
					iss.str(tokens[i]);
					std::getline(iss, token, '/');
					f.points[i - 1] = std::stoi(token) - 1;
				}
				m_Faces.push_back(f);
			}
			else if (tokens[0] == "vt") {

			}
			else if (tokens[0] == "vn") {

			}
		}
		file.close();
	}
	vec2 project(vec3 v) {

		return vec2((v.x + 1.0f) * m_Framebuffer.width() / 2.0f, (v.y + 1.0f) * m_Framebuffer.height() / 2.0f);
	}
	void draw(RenderingMode m) {

		vec3 world[3];
		vec2i screen[3];
		TGAColor c = { 255, 255, 255 };
		vec3 lightDir(0, 0, -1);

		switch (m) {
			case WIREFRAME:
				for (int i = 0; i < m_Faces.size(); i++) {
					
					for (int j = 0; j < 3; j++) {
						world[j] = m_Vertices[m_Faces[i].points[j]];
						screen[j].x = std::round((world[j].x + 1.0f) * m_Framebuffer.width() / 2.0f);
						screen[j].y = std::round((world[j].y + 1.0f) * m_Framebuffer.height() / 2.0f);
					}

					drawTriangleWireFrame(screen[0], screen[1], screen[2], m_Framebuffer, c);
				}
			case SOLID:
				for (int i = 0; i < m_Faces.size(); i++) {
					
					for (int j = 0; j < 3; j++) {
						world[j] = m_Vertices[m_Faces[i].points[j]];
						screen[j].x = std::round((world[j].x + 1.0f) * m_Framebuffer.width() / 2.0f);
						screen[j].y = std::round((world[j].y + 1.0f) * m_Framebuffer.height() / 2.0f);
					}

					vec3 normal = (world[2] - world[0]) ^ (world[1] - world[0]).normalize();
					float intensity = normal * lightDir;
					unsigned char v = intensity * 255;

					if (intensity > 0.0f)
						drawTriangleSolid(screen[0], screen[1], screen[2], m_Framebuffer, {v, v, v});
				}
		}
	}
private:

	std::vector<vec3> m_Vertices;
	std::vector<Face> m_Faces;
	TGAImage& m_Framebuffer;
};