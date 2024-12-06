#ifndef LINE_HPP
#define LINE_HPP

// My own created libraries
#include "IObject.hpp"

class Line: public IObject {
public:

    // Constructor
    Line(glm::vec3 start, glm::vec3 end, glm::vec3 col);
    // Constructor including thickness
    Line(glm::vec3 start, glm::vec3 end, glm::vec3 col, GLfloat thickness);
    // Destructor
    ~Line();

    // Creates the vertices for a triangle
    void createVertices();
    // Changes the vertices of that triangle
    void updatePosition();

private:
    // Starting position of the line
    glm::vec3 c_start;
    // Ending position of the line
    glm::vec3 c_end;
    // Color
    glm::vec3 c_color;
    // Thickness
    GLfloat c_thickness;
};

#endif