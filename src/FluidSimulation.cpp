
#include "FluidSimulation.hpp"
#include "Application.hpp"

// For assigning compute shader data
struct ComputeData {
    int numParticles; // Total number of particles
    float smoothingDistance; // Smoothing radius
    float targetDensity; // Target density
};

FluidSimulation::FluidSimulation(double u_width, double u_height, GLfloat u_smoothingDistance, Application& u_app) : app(u_app) {
    width = u_width;
    height = u_height;
    smoothingDistance = u_smoothingDistance;
}

FluidSimulation::~FluidSimulation() { }

// UTIL METHODS
// ---------------------------------------------------------
std::vector<GLfloat> linspace(GLfloat start, GLfloat end, int num) {
    std::vector<GLfloat> result(num);
    GLfloat step = (end - start) / (num - 1);

    for (int i = 0; i < num; i++) {
        result[i] = start + i * step;
    }

    return result;
}

GLfloat FluidSimulation::CalculateSharedPressure(GLfloat density1, GLfloat density2) {
    GLfloat pressure1 = density1 - targetDensity;
    GLfloat pressure2 = density2 - targetDensity;
    return (pressure1 + pressure2) / 2;
}

GLfloat FluidSimulation::smoothingKernel(GLfloat dist) {
    if (dist >= smoothingDistance) return 0;

    GLfloat volume = 15 / (glm::pi<GLfloat>() * glm::pow(smoothingDistance, 6));
    return volume * glm::pow(smoothingDistance - dist, 3);
}

GLfloat FluidSimulation::smoothingKernelDerivative(GLfloat dist) {
    if (dist >= smoothingDistance) return 0;
    GLfloat volume = -45 / (glm::pi<GLfloat>() * glm::pow(smoothingDistance, 6));
    return volume * glm::pow(smoothingDistance - dist, 2);
}

GLfloat FluidSimulation::CalculateDensity(int sampleIndex) {
    GLfloat density = 0;

    // Grab the current particle and determine what cell it's in
    FluidSimulation::Particle& currParticle = particles[sampleIndex];
    int particleIndex = 0;
    int curr_cell_index = int(CalculatePosition(currParticle));
    // The cells adjacent to check
    std::vector<FluidSimulation::Cell*> checkedCells = CreateCellsToCheck(curr_cell_index);

    for (FluidSimulation::Cell* cell: checkedCells) {
        for (FluidSimulation::Particle& particle: cell->particles) {

            // Find the index of the current particle
            auto iterator = std::find(particles.begin(), particles.end(), particle);
            if (iterator != particles.end()) {
                particleIndex = std::distance(particles.begin(), iterator);
            } else {
                std::cerr << "No particle found..." << std::endl;
            }

            GLfloat dist = glm::length(predictedPositions.at(sampleIndex) - predictedPositions.at(particleIndex));
            // GLfloat dist = glm::length(particles.at(sampleIndex).position - particles.at(i).position);
            GLfloat influence = smoothingKernel(dist);

            density += particles[sampleIndex].mass * influence;
        }
    }
    
    return density;
}
// ---------------------------------------------------------

// Draw the borders of the simulation
void FluidSimulation::DrawBorders() {
    GLfloat buffer = 0.06;
    GLfloat thickness = 0.03;
    std::shared_ptr<Line> bottomBorder = std::make_shared<Line>(
        glm::vec3(-width - buffer, -height - buffer, 0),
        glm::vec3(width + buffer, -height - buffer, 0),
        glm::vec3(1, 1, 1),
        thickness
    );
    std::shared_ptr<Line> rightBorder = std::make_shared<Line>(
        glm::vec3(width + buffer, height + buffer, 0),
        glm::vec3(width + buffer, -height - buffer, 0),
        glm::vec3(1, 1, 1),
        thickness
    );
    std::shared_ptr<Line> topBorder = std::make_shared<Line>(
        glm::vec3(width + buffer, height + buffer, 0),
        glm::vec3(-width - buffer, height + buffer, 0),
        glm::vec3(1, 1, 1),
        thickness
    );
    std::shared_ptr<Line> leftBorder = std::make_shared<Line>(
        glm::vec3(-width - buffer, height + buffer, 0),
        glm::vec3(-width - buffer, -height - buffer, 0),
        glm::vec3(1, 1, 1),
        thickness
    );

    app.AddObject(bottomBorder);
    app.AddObject(rightBorder);
    app.AddObject(topBorder);
    app.AddObject(leftBorder);
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
}

// Create the list of cells to check on every iteration
std::vector<FluidSimulation::Cell*> FluidSimulation::CreateCellsToCheck(int cellIndex) {
    int rows = (2 * height) / smoothingDistance;
    int cols = (2 * width) / smoothingDistance;
    std::vector<FluidSimulation::Cell*> cellsToCheck;

    // Offsets for the 8 neighboring cells and the current cell
    const int neighborOffsets[9] = {
        -cols - 1, -cols, -cols + 1,  // Top row
        -1,        0,      1,        // Middle row (current cell in the center)
        cols - 1,  cols,  cols + 1   // Bottom row
    };

    // Determine the current cell's row and column
    int currentRow = cellIndex / cols;
    int currentCol = cellIndex % cols;

    for (int offset : neighborOffsets) {
        int neighborIndex = cellIndex + offset;

        // Calculate the neighbor's row and column
        int neighborRow = neighborIndex / cols;
        int neighborCol = neighborIndex % cols;

        // Check if the neighbor is within bounds
        if (neighborIndex >= 0 && neighborIndex < rows * cols && 
            std::abs(currentRow - neighborRow) <= 1 && 
            std::abs(currentCol - neighborCol) <= 1) {
            
            // Add a pointer to the neighboring cell
            cellsToCheck.push_back(&cells[neighborIndex]);
        }
    }

    return cellsToCheck;
}

// Given the particle, calculate the cell it's in
int FluidSimulation::CalculatePosition(FluidSimulation::Particle& particle) {
    // Figure out how many rows there are
    int rows = round(2 * height / smoothingDistance);
    glm::vec3 position = particle.position + glm::vec3(width, height, 0);

    // Calculate the row and column the particle is in
    int col = std::floor(position.x / smoothingDistance);
    int row = rows - (std::ceil(position.y / smoothingDistance));

    // std::cout << glm::to_string(position) << std::endl;

    // Rounding correction
    if (col == round(2 * width / smoothingDistance)) {
        col -= 1;
    }
    if (row == rows) {
        row -= 1;
    }

    // std::cout << glm::to_string(position) << std::endl;
    // std::cout << "row ceil: " << std::ceil(position.y / smoothingDistance) << std::endl;
    // std::cout << col << " " << row << std::endl;
    // std::cout << col + (row * rows) << std::endl;

    return col + (rows * row);
}

// Calculate the force between 2 particles
void FluidSimulation::ComputeForce(int index, int particleIndex, glm::vec3& pressureForce) {
    // The current particle and init pressure force
    FluidSimulation::Particle& currParticle = particles[index];

    // Calculate offset, direction, density
    glm::vec3 offset = predictedPositions.at(index) - predictedPositions.at(particleIndex);
    GLfloat dist = glm::sqrt((offset.x * offset.x) + (offset.y * offset.y));
    glm::vec3 dir;
    GLfloat density = densities.at(index);

    // Pressure forces
    GLfloat slope = smoothingKernelDerivative(dist);
    GLfloat sharedPressure = CalculateSharedPressure(density, densities[index]);

    if (dist != 0) {
        dir = offset / dist;
    } else {
        dir = glm::vec3(0, 0, 0);
    }

    // // Viscosity forces
    // GLfloat mass = currParticle.mass;
    // glm::vec3 velocity_1 = particles[index].velocity;
    // glm::vec3 velocity_2 = currParticle.velocity;
    // GLfloat v_slope = SmoothingKernelViscosity(dist);
    
    // if (density != 0) {
    //     viscosity_force += mass * (velocity_2 - velocity_1) * v_slope / density 
    // }

    if (density != 0) {
        // std::cout << sharedPressure << ", " << glm::to_string(dir) << ", " << currParticle.mass << ", " << dist << ", " << slope << ", " << density << std::endl;
        pressureForce -= (sharedPressure * dir * currParticle.mass * slope / density);
        // std::cout << glm::to_string(pressureForce) << std::endl;
    }
}

// Given a particle index find the forces applied on it
glm::vec3 FluidSimulation::CalculateForces(int index) {
    // Net force
    glm::vec3 netForce = {0, 0, 0};
    // Pressure forces
    glm::vec3 pressureForce = {0, 0, 0};
    int totalParticles = 0;
    // Viscosity forces
    // glm::vec3 viscosityForce;

    // Try to do the new grid system instead of the O(n^2)
    FluidSimulation::Particle& currParticle = particles[index];
    int particleIndex = 0;
    int curr_cell_index = int(CalculatePosition(currParticle));
    std::vector<FluidSimulation::Cell*> checkedCells = CreateCellsToCheck(curr_cell_index);
    //std::cout << "Current cell index: " << curr_cell_index << std::endl;

    for (FluidSimulation::Cell* cell: checkedCells) {
        totalParticles += cell->particles.size();
        for (FluidSimulation::Particle& particle: cell->particles) {
            if (particle == currParticle) continue;
            
            //std::cout << "Calculating pressure force between " << particle._id << ", and " << currParticle._id << std::endl;
            auto iterator = std::find(particles.begin(), particles.end(), particle);
            if (iterator != particles.end()) {
                particleIndex = std::distance(particles.begin(), iterator);
            } else {
                std::cerr << "No particle found..." << std::endl;
            }

            //std::cout << glm::to_string(predictedPositions.at(index)) << ", " << glm::to_string(predictedPositions.at(particleIndex)) << std::endl;
            // Compute the force between the 2 found particles
            ComputeForce(index, particleIndex, pressureForce);
        }
    }
    //std::cout << totalParticles << std::endl;

    return pressureForce;
}

// Handle collisions between particles and walls
void FluidSimulation::HandleCollisions(int sampleIndex) {
    FluidSimulation::Particle& particle = particles[sampleIndex];
    if (glm::abs(particle.position.x) >= width) {
        //std::cout << "Left/right" << std::endl;
        particle.position.x = glm::sign(particle.position.x) * width;
        particle.velocity.x *= -dampeningConstant;
    }
    if (glm::abs(particle.position.y) >= height) {
        //std::cout << "Top/bottom" << std::endl;
        particle.position.y = glm::sign(particle.position.y) * height;
        particle.velocity.y *= -dampeningConstant;
    }
}

// Apply gravitational forces
void FluidSimulation::ApplyGravitationalForces(int sampleIndex) {
    glm::vec3 down = {0.0, -1.0, 0.0};
    FluidSimulation::Particle& currParticle = particles[sampleIndex];
    currParticle.velocity += (down * gravity * deltaTime);
    // std::cout << sampleIndex << ": " << glm::to_string(currParticle.velocity * deltaTime) << std::endl;
    predictedPositions.at(sampleIndex) = currParticle.position + (currParticle.velocity * deltaTime);
    // std::cout << sampleIndex << ": " << glm::to_string(predictedPositions[sampleIndex]) << std::endl;
    // Calculate the density between the two particles -- since there's a chance they interact
    densities.at(sampleIndex) = CalculateDensity(sampleIndex);
}

// Apply the pressure forces
void FluidSimulation::ApplyPressureForces(int sampleIndex) {
    // Get the net force from the compute shader
    // Once everything has been initialized, create and use the compute shader
    // GLuint& computeProgram = app.getComputeShader();
    // glUseProgram(computeProgram);

    // AssignComputeValues();
    // // Map the force buffer to read the computed forces
    // glBindBuffer(GL_SHADER_STORAGE_BUFFER, forceBuffer);
    // std::cout << forceBuffer << std::endl;
    // glm::vec3* result = (glm::vec3*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, particles.size() * sizeof(glm::vec3), GL_MAP_READ_BIT);

    // // Use the data
    // for (int i = 0; i < particles.size(); ++i) {
    //     glm::vec3 force = result[i];
    //     std::cout << glm::to_string(force) << std::endl;
    //     // Do something with the force (e.g., update particle velocities)
    // }

    // // Unmap the buffer after usage
    // glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    // glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    // glUseProgram(0);

    glm::vec3 netForce = CalculateForces(sampleIndex);

    glm::vec3 pressureAcceleration = glm::vec3{0, 0, 0};
        
    if (densities.at(sampleIndex) != 0) {
        pressureAcceleration = netForce / densities.at(sampleIndex);
    }

    particles.at(sampleIndex).velocity += (pressureAcceleration);
    // std::cout << glm::to_string(particles.at(sampleIndex).velocity) << std::endl;
}

// Update the positions of the particles
void FluidSimulation::UpdatePositions(int sampleIndex) {
    FluidSimulation::Particle& particle = particles[sampleIndex];
    int oldCell = CalculatePosition(particle);

    particle.position += (particle.velocity * deltaTime);
    // Update the particle collisions after the update
    HandleCollisions(sampleIndex);
    particle.position.z = 0; // Ensure no z variance

    int newCell = CalculatePosition(particle);

    // std::cout << oldCell << ", " << newCell << std::endl;

    if (newCell != oldCell) {
        // Remove the particle from the old cell
        auto& oldParticles = cells[oldCell].particles;
        // for (FluidSimulation::Particle& part: cells[oldCell].particles) {
        //     std::cout << part._id << std::endl;
        // }
        auto iterator = std::find(oldParticles.begin(), oldParticles.end(), particle);
        if (iterator != oldParticles.end()) {
            int partIndex = std::distance(oldParticles.begin(), iterator);
            oldParticles.erase(
                oldParticles.begin() + partIndex
            );
        }
        // Add it to the new one
        FluidSimulation::Particle particleCopy = particle;
        cells[newCell].particles.push_back(particleCopy);
        //std::cout << newCell << ": " << cells[newCell].particles.size() << ", " << oldCell << ": " << cells[oldCell].particles.size() << std::endl;
    }
    IObject::IPosition newPosition = IObject::CirclePosition(
        particle.position
    );
    points[sampleIndex]->updatePosition(newPosition);
    // std::cout << glm::to_string(particles[sampleIndex].position) << std::endl;
    // std::cout << glm::to_string(particles[sampleIndex].velocity) << std::endl;
}

// Set up the compute shader to have data sent to it
void FluidSimulation::AssignComputeValues() {

    // Bind the buffer for modification -- do for each buffer
    glBindBuffer(GL_UNIFORM_BUFFER, predictedPositionBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, predictedPositions.size() * sizeof(glm::vec3), &predictedPositions);

    std::vector<glm::vec3> forces(particles.size(), glm::vec3(0.0f));
    glBindBuffer(GL_UNIFORM_BUFFER, forceBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, particles.size() * sizeof(glm::vec3), &forces);

    glBindBuffer(GL_UNIFORM_BUFFER, densityBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, densities.size() * sizeof(GLfloat), &densities);

    // Bind the buffer base
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, predictedPositionBuffer);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, forceBuffer);
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, densityBuffer);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: " << error << std::endl;
    } else {
        std::cout << "No OpenGL Errors." << std::endl;
    }

    // Dispatch the compute shader
    GLuint workGroupSize = 256;  // Number of threads per workgroup
    GLuint numGroups = (particles.size() + workGroupSize - 1) / workGroupSize;  // Calculate number of groups
    glDispatchCompute(144, 1, 1);

    // Ensure the computation finishes before we read the results
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // Unbind the buffer after modification
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    std::cout << "Done initializing compute shader" << std::endl;
}

// Bind the compute buffers
void FluidSimulation::BindComputeBuffers() {
    // Use the compute shader program
    GLuint& computeProgram = app.getComputeShader();
    glUseProgram(computeProgram);

    // Check for linking errors -- verify linking went according to plan
    GLint success;
    glGetProgramiv(computeProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(computeProgram, 512, nullptr, infoLog);
        std::cerr << "Shader Program Linking Failed:\n" << infoLog << std::endl;
    }

    // Check compile errors
    glGetProgramiv(computeProgram, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(computeProgram, 512, nullptr, infoLog);
        std::cerr << "Shader Program Linking Failed:\n" << infoLog << std::endl;
        exit(EXIT_FAILURE);
    }

    // Give the shader the predicted positions
    glGenBuffers(1, &predictedPositionBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, predictedPositionBuffer);
    glBufferData(GL_UNIFORM_BUFFER, predictedPositions.size() * sizeof(glm::vec3), predictedPositions.data(), GL_DYNAMIC_DRAW);

    // Give the shader the net force
    std::vector<glm::vec3> forces(particles.size(), glm::vec3(0.0f));
    glGenBuffers(1, &forceBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, forceBuffer);
    glBufferData(GL_UNIFORM_BUFFER, forces.size() * sizeof(glm::vec3), forces.data(), GL_DYNAMIC_DRAW);

    // Give the shader the densities
    glGenBuffers(1, &densityBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, densityBuffer);
    glBufferData(GL_UNIFORM_BUFFER, densities.size() * sizeof(GLfloat), densities.data(), GL_DYNAMIC_DRAW);

    glUniform1i(glGetUniformLocation(computeProgram, "numParticles"), particles.size()); 

    // Create a buffer
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(ComputeData), nullptr, GL_DYNAMIC_DRAW);

    GLuint blockIndex = glGetUniformBlockIndex(computeProgram, "ComputeData");

    // The data to be sent to the shader
    ComputeData data = {
        static_cast<int>(particles.size()),
        smoothingDistance,
        targetDensity
    };

    // Match the uniform block binding to the compute shader
    glUniformBlockBinding(computeProgram, blockIndex, 3);

    // Assign the data
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(data), &data);

    // Unbind the buffer
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glUseProgram(0);
}

// First time render
void FluidSimulation::Render() {
    std::cout << "Rendering" << std::endl;
    // Create hash table for easy comparision of particles
    CreateHashTable();
    DrawBorders();

    // Create the grid of particles
    GLfloat lower = -width / 2;
    GLfloat upper = height / 2;
    int steps = 12;
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
            std::shared_ptr<Circle> point = std::make_shared<Circle>(
                glm::vec3(xVal, yVal, 0),
                0.04
            );
            points.push_back(point);
            app.AddObject(point);

            // Create a particle with position and velocity properties
            Particle particle = FluidSimulation::Particle(
                position,
                velocity,
                1.0f,
                id
            );

            int particleIndex = CalculatePosition(particle);
            cells[particleIndex].particles.push_back(particle);

            // Initialize all vectors to have the correct size
            predictedPositions.push_back(position);
            particles.push_back(particle);
            densities.push_back(0);
        }
    }

    BindComputeBuffers();
}

void FluidSimulation::Update() {
    for (int i = 0; i < particles.size(); i++) {
        // Handle collisions
        //HandleCollisions(i);
        // Apply the gravitational forces
        ApplyGravitationalForces(i);
        // Apply the pressure forces
        ApplyPressureForces(i);
        // Update positions
        UpdatePositions(i);
    }
}