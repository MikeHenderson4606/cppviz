#ifndef FLUIDSIMULATION_HPP
#define FLUIDSIMULATION_HPP

#include "Simulation.hpp"
#include "Circle.hpp"
#include "IObject.hpp"

class FluidSimulation: public Simulation {
public:

    struct Particle {
        Particle(glm::vec3 u_position, glm::vec3 u_velocity, GLfloat u_mass, int u_id) {
            position = u_position;
            velocity = u_velocity;
            mass = u_mass;
            _id = u_id;
        }
        glm::vec3 position;
        glm::vec3 velocity;
        GLfloat mass;
        int _id;

        bool operator==(const Particle& other) const {
            return _id == other._id;
        }
    };

    struct Cell {
        Cell(std::vector<Particle> u_particles, GLfloat u_x, GLfloat u_y, int u_id) {
            particles = u_particles;
            x = u_x;
            y = u_y;
            _id = u_id;
        }
        std::vector<Particle> particles;
        GLfloat x;
        GLfloat y;
        int _id;
    };

    FluidSimulation(double width, double height, GLfloat smoothingDistance, Application& u_app);

    ~FluidSimulation();

    /* HELPER METHODS */

    // Calculate the cell of the particle in question
    int CalculatePosition(Particle& particle);
    // Create hash table for easy particle comparison
    void CreateHashTable();
    // Used to calculate the shared pressure
    GLfloat CalculateSharedPressure(GLfloat density1, GLfloat density2);
    // The smoothing kernel
    GLfloat smoothingKernel(GLfloat dist);
    // Smoothing kernel derivative
    GLfloat smoothingKernelDerivative(GLfloat dist);
    // Calculating the density
    GLfloat CalculateDensity(int index);
    // Determine what cells need to be checked
    std::vector<Cell*> CreateCellsToCheck(int index);
    // Bind the compute buffers
    void BindComputeBuffers();
    // Using compute shders for optimization
    void AssignComputeValues();
    // Computes the force between two particles
    void ComputeForce(int index, int sampleIndex, glm::vec3& pressureForce);
    // Calculates the forces applied on a given particle
    glm::vec3 CalculateForces(int index);
    // Draw the borders of the simulation
    void DrawBorders();

    /* HELPER METHODS */

    /* UPDATE LOOP */

    // Handle collisions between walls
    void HandleCollisions(int index);
    // Apply gravitational forces
    void ApplyGravitationalForces(int index);
    // Apply the pressure forces
    void ApplyPressureForces(int index);
    // Update positions of all particles
    void UpdatePositions(int index);

    /* UPDATE LOOP*/
    
    // Initial call
    void Render() override;
    // What is called on every update
    void Update() override;

private:
    // The points as circles
    std::vector<std::shared_ptr<Circle>> points;
    // The particles' properties
    std::vector<Particle> particles;
    // Cells to efficiently calculate positions
    std::vector<Cell> cells;
    // Predicted positions
    std::vector<glm::vec3> predictedPositions;
    // Densities
    std::vector<GLfloat> densities;
    // Width
    GLfloat width;
    // Height
    GLfloat height;
    // Gravity
    GLfloat gravity = 10;
    // Dampening constant for collisions against walls
    GLfloat dampeningConstant = 0.6;
    // Time control
    float deltaTime = (1.0 / 120.0);
    // Viscoscity factor
    GLfloat mu = 0.1;
    // The smoothing distance between particles
    GLfloat smoothingDistance;
    // Target density
    GLfloat targetDensity = 20;
    // The instance of the application we are using
    Application& app;
    // Use buffers for data retrieval
    GLuint predictedPositionBuffer, forceBuffer, densityBuffer, ubo;
};


#endif