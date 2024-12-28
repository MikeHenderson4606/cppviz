# version 410 core

# define PI 3.1415926535897932384626433832795

// Thread size
layout(local_size_x = 144, local_size_y = 1, local_size_z = 1) in;

// Inputs
layout(std430, binding = 0) buffer ParticlePositions {
    vec3 predictedPositions[]; // Array of particle positions
};

layout(std430, binding = 1) buffer ForceOutput {
    vec3 forces[]; // Net force exterted by all other particles on this one
};

layout(std430, binding = 2) buffer Densities {
    float densities[];
}

layout(std140, binding = 3) uniform ComputeData {
    int numParticles; // Total number of particles
    float smoothingDistance; // Smoothing radius
    float targetDensity; // Target density
}

float SmoothingKernelDerivative(float dist) {
    if (dist >= smoothingDistance) return 0;

    float volume = -45 / (PI * pow(smoothingDistance, 6.0));
    return volume * pow(smoothingDistance - dist, 2.0);
}

float CalculateSharedPressure(float density1, float density2) {
    float pressure1 = density1 - targetDensity;
    float pressure2 = density2 - targetDensity;

    return (pressure1 + pressure2) / 2;
}

// The main computation
void main() {
    uint i = gl_GlobalInvocationID.x; // Thread ID (particle index)

    // Ensure thread is within bounds
    if (i >= numParticles) return;

    float val = numParticles * smoothingDistance * targetDensity;
    float density = densities[i];
    vec3 pos = predictedPositions[i];

    // Placeholder logic: just set the force to zero for each particle
    netForce[i] = vec3(i);
}
// void main() {
//     uint i = gl_GlobalInvocationID.x; // Thread ID (particle index)
//     if (i >= numParticles) return; // Out-of-bounds check

//     // Calculate offset, direction, density
//     for (int j = 0; j < numParticles; j++) {
//         if (i == j) continue;

//         vec3 offset = predictedPositions[i] - predictedPositions[j];
//         float dist = sqrt((offset.x * offset.x) + (offset.y * offset.y));
//         vec3 dir;
//         float density = densities[index];

//         // Pressure forces
//         float slope = SmoothingKernelDerivative(dist);
//         float sharedPressure = CalculateSharedPressure(density, densities[index]);

//         if (dist != 0) {
//             dir = offset / dist;
//         } else {
//             dir = vec3(0, 0, 0);
//         }

//         if (density != 0) {
//             atomicAdd(netForce, sharedPressure * dir * slope / density);
//         }
//     }
    
// }