#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : position(position), worldUp(up), yaw(yaw), pitch(pitch), front(glm::vec3(0.0f, 0.0f, -1.0f)),
    movementSpeed(2.5f), mouseSensitivity(0.1f), zoom(45.0f) {
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(position, position + front, up);
}

void Camera::processKeyboard(char direction, float deltaTime) {
    float velocity = movementSpeed * deltaTime;
    if (direction == 'W') // Adelante
        position += front * velocity;
    if (direction == 'S') // Atrás
        position -= front * velocity;
    if (direction == 'A') // Izquierda
        position -= right * velocity;
    if (direction == 'D') // Derecha
        position += right * velocity;

    // Agregamos nuevas teclas para rotación
    if (direction == 'Q') // Rotar hacia la izquierda
        yaw -= velocity * 50.0f;
    if (direction == 'E') // Rotar hacia la derecha
        yaw += velocity * 50.0f;
    if (direction == 'R') // Rotar hacia arriba
        pitch += velocity * 50.0f;
    if (direction == 'F') // Rotar hacia abajo
        pitch -= velocity * 50.0f;

    // Aseguramos que el "pitch" esté limitado entre -89 y 89 grados
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    updateCameraVectors();
}


void Camera::processMouseMovement(float xOffset, float yOffset, bool constrainPitch) {
    xOffset *= mouseSensitivity;
    yOffset *= mouseSensitivity;

    yaw += xOffset;
    pitch += yOffset;

    if (constrainPitch) {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::processMouseScroll(float yOffset) {
    zoom -= yOffset;
    if (zoom < 1.0f)
        zoom = 1.0f;
    if (zoom > 45.0f)
        zoom = 45.0f;
}

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    this->front = glm::normalize(front);

    right = glm::normalize(glm::cross(this->front, worldUp));
    up = glm::normalize(glm::cross(right, this->front));
}

glm::vec3 Camera::getPosition() const {
    return position;
}