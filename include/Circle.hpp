#ifndef CIRCLE_HPP
#define CIRCLE_HPP

// My own created libraries
#include "IObject.hpp"

class Circle: public IObject {
public:

    // Constructor
    Circle(glm::vec3 center, GLfloat radius);
    // Constructor including color
    Circle(glm::vec3 center, GLfloat radius, glm::vec3 color);
    // Destructor
    ~Circle();

    // Creates the vertices for a circle
    void createVertices();
    // Changes the vertices of that circle
    void updatePosition(const IPosition& u_position) override {
        if (std::holds_alternative<CirclePosition>(u_position)) {
            const CirclePosition& circlePos = std::get<CirclePosition>(u_position);
            c_center = circlePos.center;

            createVertices();
        } else {
            std::cerr << "Error: Invalid position type for Circle.\n";
    }
}

private:
    // Center of the circle
    glm::vec3 c_center;
    // Radius of the circle
    GLfloat c_radius;
    // Color of the circle
    glm::vec3 c_color;
};

#endif