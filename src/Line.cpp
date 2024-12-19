#include "Line.hpp"

// Default constructor
Line::Line(glm::vec3 start, glm::vec3 end, glm::vec3 color) {
    c_start = start;
    c_end = end;
    c_color = color;
    c_thickness = 1.0f;
    createVertices();
}

// Thickness constructor
Line::Line(glm::vec3 start, glm::vec3 end, glm::vec3 color, GLfloat thickness) {
    c_start = start;
    c_end = end;
    c_color = color;
    c_thickness = thickness;
    createVertices();
}

Line::~Line() { }

void Line::createVertices() {
    // Find perpendicular vector
    GLfloat dx = c_end.x - c_start.x;
    GLfloat dy = c_end.y - c_start.y;
    GLfloat dz = c_end.z - c_start.z;
    // Normalize to unit length
    GLfloat length = glm::sqrt((dx * dx) + (dy * dy) + (dz * dz));
    dx /= length;
    dy /= length;
    dz /= length;

    // Introduce thickness
    GLfloat offset_x = (-dy + dz) * c_thickness / 2.0f;
    GLfloat offset_y = dx * c_thickness / 2.0f;
    GLfloat offset_z = dz * c_thickness / 2.0f;

    // Create the normals from the line
    vertices = {
        c_start.x + offset_x, c_start.y + offset_y, c_start.z + offset_z,    // Normal vector start 1
        c_color.x, c_color.y, c_color.z,                                     // Color Data
        c_start.x - offset_x, c_start.y - offset_y, c_start.z - offset_z,    // Normal vector start 2
        c_color.x, c_color.y, c_color.z,                                     // Color Data
        c_end.x + offset_x, c_end.y + offset_y, c_end.z + offset_z,          // Normal vector end 1
        c_color.x, c_color.y, c_color.z,                                     // Color Data
        c_end.x - offset_x, c_end.y - offset_y, c_end.z - offset_z,          // Normal vector end 2
        c_color.x, c_color.y, c_color.z                                      // Color Data
    };

    // Create ibo for the line for efficiency
    ibo = {
        0, 1, 2,
        2, 1, 0,
        2, 1, 3,
        3, 1, 2
    };
}