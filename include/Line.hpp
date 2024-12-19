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

    // Creates the vertices for a line
    void createVertices();
    // Changes the vertices of that line
    void updatePosition(const IPosition& u_position) override {
        if (std::holds_alternative<LinePosition>(u_position)) {
            const LinePosition& linePos = std::get<LinePosition>(u_position);
            c_start = linePos.start;
            c_end = linePos.end;

            createVertices();
            std::cout << "Line position updated.\n";
        } else {
            std::cerr << "Error: Invalid position type for Line.\n";
        }
    }

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