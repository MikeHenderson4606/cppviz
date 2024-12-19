
#include "FluidSimulation.hpp"
#include "Application.hpp"


FluidSimulation::FluidSimulation(int u_width, int u_height, GLfloat u_smoothingDistance) {
    width = u_width;
    height = u_height;
    smoothingDistance = u_smoothingDistance;
}

FluidSimulation::~FluidSimulation() { }

std::vector<GLfloat> linspace(GLfloat start, GLfloat end, int num) {
    std::vector<GLfloat> result(num);
    GLfloat step = (end - start) / (num - 1);

    for (int i = 0; i < num; i++) {
        result[i] = start + i * step;
    }

    return result;
}

// Add the sim to the app
void FluidSimulation::AddApplicationToSimulation(Application* u_app) {
    if (u_app != nullptr) {
        app = u_app;
    }
}

// Create hash table for particles to live in
void FluidSimulation::CreateHashTable() {
    // Start from the top left and work right and down
    std::vector<GLfloat> cols = linspace(-width, width, round(width * 2 / smoothingDistance));
    std::vector<GLfloat> rows = linspace(height, -height, round(height * 2 / smoothingDistance));

    // Create 1D list representing a grid of cells with particles in them
    for (int i = 0; i < (cols.size() * rows.size()); i++) {
        GLfloat xVal = smoothingDistance * (i % cols.size());
        GLfloat yVal = (2 * height) - (smoothingDistance * round(i / rows.size()));

        FluidSimulation::Cell cell = FluidSimulation::Cell(
            std::vector<FluidSimulation::Particle>(),
            xVal,
            yVal,
            i
        );

        cells.push_back(cell);
    }

    std::cout << cells.size() << std::endl;
}

// Given the particle, calculate the cell it's in
int FluidSimulation::CalculatePosition(FluidSimulation::Particle particle) {
    // Figure out how many rows there are
    int rows = round(2 * height / smoothingDistance);
    glm::vec3 position = particle.position + glm::vec3(width, height, 0);

    // Calculate the row and column the particle is in
    int col = round(position.x / smoothingDistance);
    int row = rows - round(position.y / smoothingDistance) - 1;

    // Rounding correction
    if (col == round(2 * width / smoothingDistance)) {
        col = round(2 * width / smoothingDistance) - 1;
    }

    return col + (rows * row);
}

// Handle collisions between particles and walls
void FluidSimulation::HandleCollisions(int sampleIndex) {
    FluidSimulation::Particle particle = particles[sampleIndex];
    if (glm::abs(particle.position.x) >= width) {
        std::cout << "Hitting sides" << std::endl;
        particle.position.x = glm::sign(particle.position.x) * width;
        particle.velocity.x *= -dampeningConstant;
    }
    if (glm::abs(particle.position.y) >= height) {
        std::cout << "Hitting top/bottom" << std::endl;
        particle.position.y = glm::sign(particle.position.y) * height;
        particle.velocity.y *= -dampeningConstant;
    }
}

// Apply gravitational forces
void FluidSimulation::ApplyGravitationalForces(int sampleIndex) {
    glm::vec3 down = {0.0, -1.0, 0.0};
    particles[sampleIndex].velocity += down * gravity * deltaTime;
    predictedPositions[sampleIndex] = particles[sampleIndex].position + (particles[sampleIndex].velocity * deltaTime);

    // std::cout << deltaTime << std::endl;
    // std::cout << glm::to_string(particles[sampleIndex].velocity) << std::endl;
}

// Update the positions of the particles
void FluidSimulation::UpdatePositions(int sampleIndex) {
    FluidSimulation::Particle particle = particles[sampleIndex];
    int oldCell = CalculatePosition(particle);

    particle.position += particle.velocity * deltaTime;

    int newCell = CalculatePosition(particle);

    if (newCell != oldCell) {
        cells[oldCell].particles.erase(std::remove(particles.begin(), particles.end(), particle), particles.end());
        cells[newCell].particles.push_back(particle);
    }
    IObject::IPosition newPosition = IObject::CirclePosition(
        particle.position
    );
    points[sampleIndex]->updatePosition(newPosition);
    std::cout << glm::to_string(particles[sampleIndex].position) << std::endl;
    std::cout << glm::to_string(particles[sampleIndex].velocity) << std::endl;
}

// First time render
void FluidSimulation::Render() {
    std::cout << "Rendering" << std::endl;
    // Create hash table for easy comparision of particles
    CreateHashTable();

    // Create the grid of particles
    GLfloat lower = -width / 2;
    GLfloat upper = height / 2;
    int steps = 10;
    std::vector<GLfloat> xValues = linspace(lower, upper, steps);
    std::vector<GLfloat> yValues = linspace(lower, upper, steps);

    for (int i = 0; i < yValues.size(); i++) {
        for (int j = 0; j < xValues.size(); j++) {
            int id = j + (xValues.size() * i);
            GLfloat xVal = xValues[j];
            GLfloat yVal = yValues[i];

            glm::vec3 position = {xVal, yVal, 0};
            glm::vec3 velocity = {0, 0, 0};
            
            // Create a circle representing a point
            Circle* point = new Circle(
                glm::vec3(xVal, yVal, 0),
                0.05
            );
            points.push_back(point);
            app->AddObject(point);

            // Create a particle with position and velocity properties
            Particle particle = FluidSimulation::Particle(
                position,
                velocity,
                1.0f,
                id
            );

            int particleIndex = CalculatePosition(particle);
            cells[particleIndex].particles.push_back(particle);

            predictedPositions.push_back(position);
            particles.push_back(particle);
        }
    }
}

void FluidSimulation::Update() {
    for (int i = 0; i < particles.size(); i++) {
        // Handle collisions
        HandleCollisions(i);
        // Apply the gravitational forces
        ApplyGravitationalForces(i);
        // Update positions
        UpdatePositions(i);
    }
}