// Include my libaries
#include "SDLGraphicsProgram.hpp"
#include "IObject.hpp"
#include "Triangle.hpp"
#include "Line.hpp"
#include "Circle.hpp"

// C++ Standard Libraries
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

class Application{
public:

    // Constructor
    Application(SDLGraphicsProgram& program);
    // Destructor
    ~Application();

    // Pre loop
    void PreLoop();
    // Adds objects to the scene
    void AddObject(IObject* object);
    // Handles input
    void HandleInput(SDL_Event e);
    // Updates objects
    void Update();
    // Renders objects
    void Render();
    // Draws an object
    void Draw(std::vector<GLint> &indices);

private:

    // The program we are running
    SDLGraphicsProgram& program;
    // We use a VAO
    GLuint vao = 0;
    // And a vbo
    GLuint vbo = 0;
    // A default shader
    GLuint defaultShader;

    // RENDER ALL OBJECTS AT ONCE
    // Triangles in scene
    std::vector<Triangle*> triangles;
    std::vector<GLfloat> triangleVertices;
    std::vector<GLint> triangleIndices;
    // Lines in scene
    std::vector<Line*> lines;
    std::vector<GLfloat> lineVertices;
    std::vector<GLint> lineIndices;
    // Circles in scene
    std::vector<Circle*> circles;
    std::vector<GLfloat> circleVertices;
    std::vector<GLint> circleIndices;

};