#ifndef IOBJECT_HPP
#define IOBJECT_HPP
// Third party libraries
#include <glad/glad.h> // The glad library helps setup OpenGL extensions.
#include <glm/glm.hpp>

// C++ Standard Libraries
#include <iostream>
#include <variant>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>

class IObject {
public:

    struct TrianglePosition {
        TrianglePosition(glm::vec3 u_pos1, glm::vec3 u_pos2, glm::vec3 u_pos3) {
            pos1 = u_pos1;
            pos2 = u_pos2;
            pos3 = u_pos3;
        }
        glm::vec3 pos1;
        glm::vec3 pos2;
        glm::vec3 pos3;
    };

    struct CirclePosition {
        CirclePosition(glm::vec3 u_center) {
            center = u_center;
        }
        glm::vec3 center;
    };

    struct LinePosition {
        LinePosition(glm::vec3 u_start, glm::vec3 u_end) {
            start = u_start;
            end = u_end;
        }
        glm::vec3 start;
        glm::vec3 end;
    };

    using IPosition = std::variant<TrianglePosition, CirclePosition, LinePosition>;

    // Constructor
    IObject() {}
    // Destructor
    virtual ~IObject() {}

    // Creates the vertices
    virtual void createVertices() = 0;
    // Updates the vertices
    virtual void updatePosition(const IPosition& newPosition) = 0;

    // Vertices of the object
    std::vector<GLfloat> vertices;
    // IBO used in drawing
    std::vector<GLuint> ibo;
};

#endif // IOBJECT_HPP