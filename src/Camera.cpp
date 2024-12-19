#include "Camera.hpp"


Camera::Camera() {
}

Camera::~Camera() { }

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(
        eyePosition,
        eyePosition + viewDirection,
        upVector
    );
}

void Camera::SetCameraEyePosition(GLfloat x, GLfloat y, GLfloat z) {
    eyePosition.x = x;
    eyePosition.y = y;
    eyePosition.z = z;
}
