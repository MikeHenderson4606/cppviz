#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

// My own created libraries
#include "IObject.hpp"

class Triangle: public IObject {
public:

    // Constructor
    Triangle(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3);
    // Destructor
    ~Triangle();

    // Creates the vertices for a triangle
    void createVertices();
    // Changes the vertices of that triangle
    void updatePosition(const IPosition& position) override {
        if (std::holds_alternative<TrianglePosition>(position)) {
            const TrianglePosition& trianglePos = std::get<TrianglePosition>(position);
            pos1 = trianglePos.pos1;
            pos2 = trianglePos.pos2;
            pos3 = trianglePos.pos3;

            createVertices();
            std::cout << "Triangle position updated.\n";
        } else {
            std::cerr << "Error: Invalid position type for Triangle.\n";
        }
    }

private:
    // First position
    glm::vec3 pos1;
    // Second position
    glm::vec3 pos2;
    // Third position
    glm::vec3 pos3;
};

#endif