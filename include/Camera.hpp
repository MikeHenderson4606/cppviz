#ifndef CAMERA_HPP
#define CAMERA_HPP

// Third party libraries
#define GLM_ENABLE_EXPERIMENTAL
#include <glad/glad.h> // The glad library helps setup OpenGL extensions.
#include <glm/glm.hpp>
#include "glm/gtx/transform.hpp"

// C++ Standard Libraries
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

class Camera {
public:
    // Constructor
    Camera();
    // Destructor
    ~Camera();

    // Gets the view matrix for the current camera configuration
    glm::mat4 GetViewMatrix() const;
    // Sets the eye position of the camera
    void SetCameraEyePosition(GLfloat x, GLfloat y, GLfloat z);

private:
    // Current position of the camera in world space
    glm::vec3 eyePosition = {
        0.0f, 0.0f, 8.0f
    };
    // Where the camera is currently looking
    glm::vec3 viewDirection = {
        0.0, 0.0, -1.0f
    };
    // Where "up" is (usually along y axis)
    glm::vec3 upVector = {
        0.0, 1.0f, 0.0
    };
};

#endif