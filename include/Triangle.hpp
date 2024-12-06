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
    void updatePosition();

private:
    // First position
    glm::vec3 pos1;
    // Second position
    glm::vec3 pos2;
    // Third position
    glm::vec3 pos3;
};

#endif