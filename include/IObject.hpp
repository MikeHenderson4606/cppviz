#ifndef IOBJECT_HPP
#define IOBJECT_HPP
// Third party libraries
#include <glad/glad.h> // The glad library helps setup OpenGL extensions.
#include <glm/glm.hpp>

// C++ Standard Libraries
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

class IObject {
public:

    // Constructor
    IObject() {}
    // Destructor
    virtual ~IObject() {}

    // Creates the vertices
    virtual void createVertices() = 0;
    // Updates the vertices
    virtual void updatePosition() = 0;

    // Vertices of the object
    std::vector<GLfloat> vertices;
    // IBO used in drawing
    std::vector<GLuint> ibo;
};

#endif // IOBJECT_HPP