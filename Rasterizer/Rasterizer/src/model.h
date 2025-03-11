#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "tgaimage.h"
#include "draw.h"

class vec2 {

public:

	float x, y;
	
	vec2() {}
	vec2(float x) : x(x), y(x) {}
	vec2(float x, float y) : x(x), y(y) {}

	float& operator[](int i) {
		
		switch (i) {
		case 0:
			return x;
		case 1:
			return y;
		default:
			throw std::invalid_argument("Subscript out of range!\n");
		}
	}
};

class vec3 {
	
public:
	
	float x, y, z;

	vec3() {}
	vec3(float x) : x(x), y(x), z(x) {}
	vec3(float x, float y, float z): x(x), y(y), z(z) {}

	
	float& operator[](int i) {
		switch (i) {
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		default:
			throw std::invalid_argument("Subscript out of range!\n");
		}
	}
};

struct Face {
	
	int points[3];

	Face() {}

	Face(int p1, int p2, int p3) {
	
		points[0] = p1, points[0] = p2, points[0] = p3;
	}

};

class model {

public:

	model(std::string location, TGAColor color, TGAImage& framebuffer) : m_Color(color), m_Framebuffer(framebuffer) {
		
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
					v[i - 1] = std::stof(tokens[i]);
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
	void draw() {

		for (int i = 0; i < m_Faces.size(); i++) {
			vec2 v[3];
			v[0] = project(m_Vertices[m_Faces[i].points[0]]);
			v[1] = project(m_Vertices[m_Faces[i].points[1]]);
			v[2] = project(m_Vertices[m_Faces[i].points[2]]);

			drawTriangleWireFrame(std::round(v[0].x), std::round(v[0].y), std::round(v[1].x), std::round(v[1].y), 
				std::round(v[2].x), std::round(v[2].y), m_Framebuffer, m_Color);
		}
	}
private:

	std::vector<vec3> m_Vertices;
	std::vector<Face> m_Faces;
	TGAColor m_Color;
	TGAImage& m_Framebuffer;
};