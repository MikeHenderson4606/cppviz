#include "Circle.hpp"
#define PI 3.14159265358979323846264338327950288

// Default constructor
Circle::Circle(glm::vec3 center, GLfloat radius) {
    c_center = center;
    c_radius = radius;
    c_color = glm::vec3(1.0f, 1.0f, 1.0f);
    createVertices();
}

// Color constructor
Circle::Circle(glm::vec3 center, GLfloat radius, glm::vec3 color) {
    c_center = center;
    c_radius = radius;
    c_color = color;
    createVertices();
}

Circle::~Circle() { }

void Circle::createVertices() {
    // Determine the circle quality
    int circleQuality = 30;
    GLfloat interval = (2 * PI) / circleQuality;

    // Add the center position with color
    vertices.push_back(c_center.x);
    vertices.push_back(c_center.y);
    vertices.push_back(c_center.z);
    vertices.push_back(c_color.x);
    vertices.push_back(c_color.y);
    vertices.push_back(c_color.z);

    for (int i = 0; i < circleQuality + 2; i++) {
        GLfloat angle = i * interval;
        GLfloat x = c_center.x + (c_radius * glm::cos(angle));
        GLfloat y = c_center.y + (c_radius * glm::sin(angle));
        GLfloat z = c_center.z;

        // Push back vertices
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(z);
        vertices.push_back(c_color.x);
        vertices.push_back(c_color.y);
        vertices.push_back(c_color.z);

        // Push back indices
        if (i > 1) {
            ibo.push_back(0);        // Center vertex
            ibo.push_back(i - 1);    // Segment 1
            ibo.push_back(i);        // Segment 2
        }
    }
}

void Circle::updatePosition() {

}