#include "Triangle.hpp"

Triangle::Triangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
    pos1 = p1;
    pos2 = p2;
    pos3 = p3;
    createVertices();
}

Triangle::~Triangle() { }

void Triangle::createVertices() {
    vertices = {
        pos1.x, pos1.y, pos1.z,     // Position
        1.0f, 0.0f, 0.0f,           // Color
        pos2.x, pos2.y, pos2.z,     // Position
        1.0f, 0.0f, 0.0f,           // Color
        pos3.x, pos3.y, pos3.z,     // Position
        1.0f, 0.0f, 0.0f,           // Color
    };

    // The ibo which tells the gpu which vertices to draw in which order
    ibo = {
        0, 1, 2, 
        2, 1, 0
    };
}