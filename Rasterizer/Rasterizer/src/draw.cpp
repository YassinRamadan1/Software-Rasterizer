#include "draw.h"

void drawLine(glm::ivec2 v1, glm::ivec2 v2, TGAImage& framebuffer, TGAColor c) {

    bool isTransposed = false;
    if (abs(v2.x - v1.x) < abs(v2.y - v1.y)) { // transpose 
        std::swap(v1.x, v1.y);
        std::swap(v2.x, v2.y);
        isTransposed = true;
    }
    if (v2.x - v1.x < 0) { // draw from left to right
        std::swap(v1.x, v2.x);
        std::swap(v1.y, v2.y);
    }

    int dx = v2.x - v1.x, dy = v2.y - v1.y, x = v1.x, y = v1.y;

    if (isTransposed)
        framebuffer.set(y, x, c);
    else
        framebuffer.set(x, y, c);

    if (dy < 0) {

        int d = -dx - 2 * dy, d1 = -2 * dy, d2 = -2 * (dx + dy);

        while (x < v2.x) {
            x++;
            if (d < 0)
                d += d1;
            else
                y--, d += d2;
            if (isTransposed)
                framebuffer.set(y, x, c);
            else
                framebuffer.set(x, y, c);
        }
    }
    else {

        int d = dx - 2 * dy, d1 = -2 * dy, d2 = 2 * (dx - dy);
        while (x < v2.x) {
            x++;
            if (d > 0)
                d += d1;
            else
                y++, d += d2;
            if (isTransposed)
                framebuffer.set(y, x, c);
            else
                framebuffer.set(x, y, c);
        }
    }
}

void drawTriangleWireFrame(glm::ivec2 v1, glm::ivec2 v2, glm::ivec2 v3, TGAImage& framebuffer, TGAColor c) {

    drawLine(v1, v2, framebuffer, c);
    drawLine(v2, v3, framebuffer, c);
    drawLine(v3, v1, framebuffer, c);
}

void drawScanLine(int x1, int x2, int y, TGAImage& framebuffer, TGAColor c) {

    if (x1 > x2)
        std::swap(x1, x2);
    while (x1 <= x2)
        framebuffer.set(x1++, y, c);
}


std::vector<utility::Triangle> clipFarPlane(utility::Triangle& triangle) {

    std::vector<utility::Triangle> triangles;
    bool state[3] = { false, false, false };
    // relax the comparison (consider z value equals the near plane)
    state[0] = (triangle.position[0].z > triangle.position[0].w - utility::EPSILON),
        state[1] = (triangle.position[1].z > triangle.position[1].w - utility::EPSILON),
        state[2] = (triangle.position[2].z > triangle.position[2].w - utility::EPSILON);
    int sum = state[0] + state[1] + state[2];

    if (sum == 3) { // the whole triangle is rejected
        return triangles;
    }

    if (!sum) {   // the whole triangle is accepted

        triangles.push_back(triangle);
        return triangles;
    }

    if (sum == 1) { // one vertex is clipped

        // make state[0] always true (has to be clipped)
        if (state[1]) {
            std::swap(triangle.position[0], triangle.position[1]);
            std::swap(triangle.textureCoord[0], triangle.textureCoord[1]);
            std::swap(triangle.color[0], triangle.color[1]);
            std::swap(state[0], state[1]);
        }
        else if (state[2]) {
            std::swap(triangle.position[0], triangle.position[2]);
            std::swap(triangle.textureCoord[0], triangle.textureCoord[2]);
            std::swap(triangle.color[0], triangle.color[2]);
            std::swap(state[0], state[2]);
        }

        // find intersection 1
        float t1 = (-triangle.position[0].z + triangle.position[0].w) / (triangle.position[1].z - triangle.position[1].w - triangle.position[0].z + triangle.position[0].w);
        glm::vec4 position1 = triangle.position[0] + t1 * (triangle.position[1] - triangle.position[0]);
        glm::vec3 textureCoord1 = triangle.textureCoord[0] + t1 * (triangle.textureCoord[1] - triangle.textureCoord[0]);
        glm::vec4 color1 = triangle.color[0] + t1 * (triangle.color[1] - triangle.color[0]);
        utility::Triangle triangle1 = triangle;
        triangle1.position[0] = position1;
        triangle1.textureCoord[0] = textureCoord1;
        triangle1.color[0] = color1;
        triangles.push_back(triangle1);

        // find intersection 2
        float t2 = (-triangle.position[0].z + triangle.position[0].w) / (triangle.position[2].z - triangle.position[2].w - triangle.position[0].z + triangle.position[0].w);
        glm::vec4 position2 = triangle.position[0] + t2 * (triangle.position[2] - triangle.position[0]);
        glm::vec3 textureCoord2 = triangle.textureCoord[0] + t2 * (triangle.textureCoord[2] - triangle.textureCoord[0]);
        glm::vec4 color2 = triangle.color[0] + t2 * (triangle.color[2] - triangle.color[0]);
        utility::Triangle triangle2 = triangle1;
        triangle2.position[1] = position2;
        triangle2.textureCoord[1] = textureCoord2;
        triangle2.color[1] = color2;
        triangles.push_back(triangle2);

        return triangles;
    }
    // two vertices are clipped

    // make state[0] always false (the other two vertices have to be clipped)
    if (state[0]) {

        std::swap(triangle.position[0], triangle.position[1]);
        std::swap(triangle.textureCoord[0], triangle.textureCoord[1]);
        std::swap(triangle.color[0], triangle.color[1]);
        std::swap(state[0], state[1]);
    }
    if (state[0]) {

        std::swap(triangle.position[0], triangle.position[2]);
        std::swap(triangle.textureCoord[0], triangle.textureCoord[2]);
        std::swap(triangle.color[0], triangle.color[2]);
        std::swap(state[0], state[2]);
    }

    // find intersection 1
    float t1 = (-triangle.position[1].z + triangle.position[1].w) / (triangle.position[0].z - triangle.position[0].w - triangle.position[1].z + triangle.position[1].w);
    glm::vec4 position1 = triangle.position[1] + t1 * (triangle.position[0] - triangle.position[1]);
    glm::vec3 textureCoord1 = triangle.textureCoord[1] + t1 * (triangle.textureCoord[0] - triangle.textureCoord[1]);
    glm::vec4 color1 = triangle.color[1] + t1 * (triangle.color[0] - triangle.color[1]);
    utility::Triangle triangle1 = triangle;
    triangle1.position[1] = position1;
    triangle1.textureCoord[1] = textureCoord1;
    triangle1.color[1] = color1;

    // find intersection 2
    float t2 = (-triangle.position[2].z + triangle.position[2].w) / (triangle.position[0].z - triangle.position[0].w - triangle.position[2].z + triangle.position[2].w);
    glm::vec4 position2 = triangle.position[2] + t2 * (triangle.position[0] - triangle.position[2]);
    glm::vec3 textureCoord2 = triangle.textureCoord[2] + t2 * (triangle.textureCoord[0] - triangle.textureCoord[2]);
    glm::vec4 color2 = triangle.color[2] + t2 * (triangle.color[0] - triangle.color[2]);
    triangle1.position[2] = position2;
    triangle1.textureCoord[2] = textureCoord2;
    triangle1.color[2] = color2;

    triangles.push_back(triangle1);

    return triangles;
}

std::vector<utility::Triangle> clipNearPlane(utility::Triangle& triangle) {

    std::vector<utility::Triangle> triangles;
    bool state[3] = { false, false, false };
    // relax the comparison (consider z value equals the near plane)
    state[0] = (triangle.position[0].z < -triangle.position[0].w + utility::EPSILON);
    state[1] = (triangle.position[1].z < -triangle.position[1].w + utility::EPSILON);
    state[2] = (triangle.position[2].z < -triangle.position[2].w + utility::EPSILON);

    int sum = state[0] + state[1] + state[2];
    if (sum == 3) { // the whole triangle is rejected
        return triangles;
    }

    if (!sum) {   // the whole triangle is accepted

        triangles.push_back(triangle);
        return triangles;
    }

    if (sum == 1) { // one vertex is clipped

        // make state[0] always true (P0 has to be clipped)
        if (state[1]) {
            std::swap(triangle.position[0], triangle.position[1]);
            std::swap(triangle.textureCoord[0], triangle.textureCoord[1]);
            std::swap(triangle.color[0], triangle.color[1]);
            std::swap(state[0], state[1]);
        }
        else if (state[2]) {
            std::swap(triangle.position[0], triangle.position[2]);
            std::swap(triangle.textureCoord[0], triangle.textureCoord[2]);
            std::swap(triangle.color[0], triangle.color[2]);
            std::swap(state[0], state[2]);
        }

        // find intersection 1 between P0 and P1
        float t1 = -(triangle.position[0].z + triangle.position[0].w) / (triangle.position[1].z + triangle.position[1].w - triangle.position[0].z - triangle.position[0].w);
        glm::vec4 position1 = triangle.position[0] + t1 * (triangle.position[1] - triangle.position[0]);
        glm::vec3 textureCoord1 = triangle.textureCoord[0] + t1 * (triangle.textureCoord[1] - triangle.textureCoord[0]);
        glm::vec4 color1 = triangle.color[0] + t1 * (triangle.color[1] - triangle.color[0]);
        utility::Triangle triangle1 = triangle; // triangle1 is P0, P1, P2
        triangle1.position[0] = position1;
        triangle1.textureCoord[0] = textureCoord1;
        triangle1.color[0] = color1;
        triangles.push_back(triangle1); // triangle1 is newP0 along edge(P0, P1), P1, P2

        // find intersection 2 between P0 and P2
        float t2 = -(triangle.position[0].z + triangle.position[0].w) / (triangle.position[2].z + triangle.position[2].w - triangle.position[0].z - triangle.position[0].w);
        glm::vec4 position2 = triangle.position[0] + t2 * (triangle.position[2] - triangle.position[0]);
        glm::vec3 textureCoord2 = triangle.textureCoord[0] + t2 * (triangle.textureCoord[2] - triangle.textureCoord[0]);
        glm::vec4 color2 = triangle.color[0] + t2 * (triangle.color[2] - triangle.color[0]);
        utility::Triangle triangle2 = triangle1; // triangle2 is newP0 along edge(P0, P1), P1, P2
        triangle2.position[1] = position2;
        triangle2.textureCoord[1] = textureCoord2;
        triangle2.color[1] = color2;
        triangles.push_back(triangle2); // triangle2 is newP0 along edge(P0, P1), newP1 along edge(P0, P2), P2

        return triangles;
    }
    // two vertices are clipped

    // make state[0] always false (the other two vertices have to be clipped)
    if (state[0]) {

        std::swap(triangle.position[0], triangle.position[1]);
        std::swap(triangle.textureCoord[0], triangle.textureCoord[1]);
        std::swap(triangle.color[0], triangle.color[1]);
        std::swap(state[0], state[1]);
    }
    if (state[0]) {

        std::swap(triangle.position[0], triangle.position[2]);
        std::swap(triangle.textureCoord[0], triangle.textureCoord[2]);
        std::swap(triangle.color[0], triangle.color[2]);
        std::swap(state[0], state[2]);
    }

    // find intersection 1 between P1 and P0
    float t1 = -(triangle.position[1].z + triangle.position[1].w) / (triangle.position[0].z + triangle.position[0].w - triangle.position[1].z - triangle.position[1].w);
    glm::vec4 position1 = triangle.position[1] + t1 * (triangle.position[0] - triangle.position[1]);
    glm::vec3 textureCoord1 = triangle.textureCoord[1] + t1 * (triangle.textureCoord[0] - triangle.textureCoord[1]);
    glm::vec4 color1 = triangle.color[1] + t1 * (triangle.color[0] - triangle.color[1]);
    utility::Triangle triangle1 = triangle; // triangle1 P0, P1, P2
    triangle1.position[1] = position1;
    triangle1.textureCoord[1] = textureCoord1;
    triangle1.color[1] = color1; // triangle1 P0, newP1 along edge(P1, P0), P2

    // find intersection 2 between P2 and P0
    float t2 = -(triangle.position[2].z + triangle.position[2].w) / (triangle.position[0].z + triangle.position[0].w - triangle.position[2].z - triangle.position[2].w);
    glm::vec4 position2 = triangle.position[2] + t2 * (triangle.position[0] - triangle.position[2]);
    glm::vec3 textureCoord2 = triangle.textureCoord[2] + t2 * (triangle.textureCoord[0] - triangle.textureCoord[2]);
    glm::vec4 color2 = triangle.color[2] + t2 * (triangle.color[0] - triangle.color[2]);
    triangle1.position[2] = position2;
    triangle1.textureCoord[2] = textureCoord2;
    triangle1.color[2] = color2; // triangle1 P0, newP1 along edge(P1, P0), newP2 along edge(P2, P0)

    triangles.push_back(triangle1);

    return triangles;
}

std::vector<utility::Triangle> clip(utility::Triangle& triangle) {

    std::vector<utility::Triangle> temp = clipNearPlane(triangle), temp2, triangles;
    for (int i = 0; i < temp.size(); i++) {
        temp2 = clipFarPlane(temp[i]);
		for (int j = 0; j < temp2.size(); j++)
			triangles.push_back(temp2[j]);
    }
    return triangles;
}

void drawTriangleSolid(utility::Triangle& triangle, TGAImage& tex, float* zBuffer, TGAImage& framebuffer, bool projectionType, float n, float f) {

    glm::vec2 v0 = glm::vec2(triangle.position[0]), v1 = glm::vec2(triangle.position[1]), v2 = glm::vec2(triangle.position[2]);

    // get the bounding box of the triangle and clip it to the framebuffer (clip against the left, right, bottom and top planes)

    int xmin = std::max(0.0f, std::round(std::min(v0.x, std::min(v1.x, v2.x)))),
        ymin = std::max(0.0f, std::round(std::min(v0.y, std::min(v1.y, v2.y)))),
        xmax = std::min(framebuffer.width() - 1.0f, std::round(std::max(v0.x, std::max(v1.x, v2.x)))),
        ymax = std::min(framebuffer.height() - 1.0f, std::round(std::max(v0.y, std::max(v1.y, v2.y))));

    double area = utility::cross(v0, v1, v2);

    float z0 = (projectionType == false) * triangle.position[0].z + (projectionType == true) * (f * n) / (f - triangle.position[0].z * (f - n)),
        z1 = (projectionType == false) * triangle.position[1].z + (projectionType == true) * (f * n) / (f - triangle.position[1].z * (f - n)),
        z2 = (projectionType == false) * triangle.position[2].z + (projectionType == true) * (f * n) / (f - triangle.position[2].z * (f - n));

    // it is an instruction to OpenMP to parallelize the loop since it is perfectly parallel
#pragma omp parallel for
    for (int y = ymin; y < ymax + 1; y++) {
        int index = y * framebuffer.width();
        for (int x = xmin; x < xmax + 1; x++) {

            // get the barycentric coordinates alpha * P0 + beta * P1 + gamma * P2
            // watch out for orientation!
            double alpha = utility::cross(glm::vec2(x, y), v1, v2) / area,
                beta = utility::cross(glm::vec2(x, y), v2, v0) / area,
                gamma = utility::cross(glm::vec2(x, y), v0, v1) / area;

            if (alpha < 0.0f || beta < 0.0f || gamma < 0.0f)
                continue;

            // early depth testing
            float z_interp = alpha * z0 + beta * z1 + gamma * z2;
            if (projectionType == true) {
                z_interp = alpha * 1 / z0 + beta * 1 / z1 + gamma * 1 / z2;
                z_interp = 1 / z_interp;
			}

            if (z_interp < zBuffer[index + x]) {

                float r_interp, g_interp, b_interp;
                unsigned char r = 0, g = 0, b = 0;
                if (projectionType == true) {
                    r_interp = alpha * triangle.color[0].r + beta * triangle.color[1].r + gamma * triangle.color[2].r;
                    r_interp *= z_interp;
                    g_interp = alpha * triangle.color[0].g + beta * triangle.color[1].g + gamma * triangle.color[2].g;
                    g_interp *= z_interp;
                    b_interp = alpha * triangle.color[0].b + beta * triangle.color[1].b + gamma * triangle.color[2].b;
                    b_interp *= z_interp;

                    // map the z_interp to the viewport space just for consistency

                }
                else {
					r_interp = alpha * triangle.color[0].r + beta * triangle.color[1].r + gamma * triangle.color[2].r;
					g_interp = alpha * triangle.color[0].g + beta * triangle.color[1].g + gamma * triangle.color[2].g;
					b_interp = alpha * triangle.color[0].b + beta * triangle.color[1].b + gamma * triangle.color[2].b;
				}

                // clamp the color values
                r = static_cast<unsigned char>(std::clamp(static_cast<int>(std::round(r_interp)), 0, 255));
                g = static_cast<unsigned char>(std::clamp(static_cast<int>(std::round(g_interp)), 0, 255));
                b = static_cast<unsigned char>(std::clamp(static_cast<int>(std::round(b_interp)), 0, 255));
                
                // set the framebuffer color
                framebuffer.set(x, y, {b, g, r});

                // update the zBuffer
                zBuffer[index + x] = z_interp;
            }

        }
    }
}

void rasterize(utility::Triangle triangle,utility::Transform& transform, TGAImage& texture, float* zBuffer, TGAImage& framebuffer) {
    
    // perspective true, orthographic false
    // knowing the projection type is important for knowing how to interpolate the attribute values

    bool projectionType = (abs(transform.projection[3][2]) < utility::EPSILON);
    glm::mat4 mvp = transform.getModelViewProjection();
    std::vector<utility::Triangle> triangles;

    // apply the transformations
    for(int i = 0; i < 3; i++) {
		triangle.position[i] =  mvp * triangle.position[i];
	}
/*
	// backface culling
    float area = glm::length(glm::cross(glm::vec3(triangle.position[1]) - glm::vec3(triangle.position[0]),
        glm::vec3(triangle.position[2]) - glm::vec3(triangle.position[0])));
    
    if (area < 1.0f)
        return;
*/

    // clip the triangle against the near and far planes
    triangles = clip(triangle);

    // perform homogenous division and then viewport transformation
    for (auto& triangle : triangles) {

        for (int i = 0; i < 3; i++) {
            triangle.position[i] /= triangle.position[i].w;
            triangle.position[i] = transform.viewport * triangle.position[i];
        }
        
        // rasterize the triangle
        drawTriangleSolid(triangle, texture, zBuffer, framebuffer, projectionType, transform.near, transform.far);
    }
}
