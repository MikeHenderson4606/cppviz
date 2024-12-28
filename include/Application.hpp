// Include my libaries
#include "SDLGraphicsProgram.hpp"
#include "IObject.hpp"
#include "Triangle.hpp"
#include "Line.hpp"
#include "Circle.hpp"
#include "Camera.hpp"

// glm libraries
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/string_cast.hpp>


// C++ Standard Libraries
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <thread>

class Simulation;

class Application{
public:

    // Constructor
    Application(SDLGraphicsProgram& program);
    // Destructor
    ~Application();

    // Pre loop
    void PreLoop();
    // Gets the compute shader
    GLuint& getComputeShader();
    // Adds objects to the scene
    void AddObject(std::shared_ptr<IObject> object);
    // Adds a simulation to the scene
    void AddSimulation(Simulation* sim);
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
    // The compute shader for quick rendering
    GLuint computeShader;

    // CAMERA
    Camera* camera;

    // SIMULATIONS
    std::vector<Simulation*> simulations;

    // RENDER ALL OBJECTS AT ONCE
    // Triangles in scene
    std::vector<std::shared_ptr<Triangle>> triangles;
    std::vector<GLfloat> triangleVertices;
    std::vector<GLint> triangleIndices;
    // Lines in scene
    std::vector<std::shared_ptr<Line>> lines;
    std::vector<GLfloat> lineVertices;
    std::vector<GLint> lineIndices;
    // Circles in scene
    std::vector<std::shared_ptr<Circle>> circles;
    std::vector<GLfloat> circleVertices;
    std::vector<GLint> circleIndices;

};