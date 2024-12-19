
#ifndef SIMULATION_HPP
#define SIMULATION_HPP

// Third party libraries
#include <glad/glad.h> // The glad library helps setup OpenGL extensions.
#include <glm/glm.hpp>

// C++ Standard Libraries
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

class Application;

class Simulation {
public:
    // Add the application to the simulation
    virtual void AddApplicationToSimulation(Application* app) = 0;
    // Initial call of the simulation
    virtual void Render() = 0;
    // What to update on every subsequent step of the simulation
    virtual void Update() = 0;
};

#endif