#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

enum class Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

//Default parameters
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.075f;
const float ZOOM = 45.0f;


class Camera
{
public:
    // Camera
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // Angles
    float Yaw;
    float Pitch;

    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    struct Limits {
        glm::vec3 negCorner;
        glm::vec3 posCorner;
    } cameraLimits;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), 
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), 
           float yaw = YAW, float pitch = PITCH,
           glm::vec3 negCorner = glm::vec3(-10.0f, -3.0f, -10.0f),
           glm::vec3 posCorner = glm::vec3(10.0f, 10.0f, 10.0f)) :
           Front{ glm::vec3(0.0f, 0.0f, -1.0f) },
           MovementSpeed{ SPEED }, 
           MouseSensitivity{ SENSITIVITY }, 
           Zoom{ ZOOM },
           cameraLimits{ negCorner, posCorner}
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    Camera(float posX, float posY, float posZ, 
           float upX, float upY, float upZ, 
           float yaw, float pitch,
           glm::vec3 negCorner = glm::vec3(-10.0f, -3.0f, -10.0f),
           glm::vec3 posCorner = glm::vec3(10.0f, 10.0f, 10.0f)) :
           Front{ glm::vec3(0.0f, 0.0f, -1.0f) }, 
           MovementSpeed{ SPEED }, 
           MouseSensitivity{ SENSITIVITY }, 
           Zoom{ ZOOM },
           cameraLimits{ negCorner, posCorner }
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == Camera_Movement::FORWARD) {
            Position += Front * velocity;
        }
        if (direction == Camera_Movement::BACKWARD) {
            Position -= Front * velocity;
        }
        if (direction == Camera_Movement::LEFT) {
            Position -= Right * velocity;
        }
        if (direction == Camera_Movement::RIGHT) {
            Position += Right * velocity;
        }
        Position.x = Position.x < cameraLimits.negCorner.x ? cameraLimits.negCorner.x : Position.x;
        Position.x = Position.x > cameraLimits.posCorner.x ? cameraLimits.posCorner.x : Position.x;
        Position.y = Position.y < cameraLimits.negCorner.y ? cameraLimits.negCorner.y : Position.y;
        Position.y = Position.y > cameraLimits.posCorner.y ? cameraLimits.posCorner.y : Position.y;
        Position.z = Position.z < cameraLimits.negCorner.z ? cameraLimits.negCorner.z : Position.z;
        Position.z = Position.z > cameraLimits.posCorner.z ? cameraLimits.posCorner.z : Position.z;
    }

    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        if (constrainPitch)
        {
            Pitch = Pitch > 89.0f ? 89.0f : Pitch;
            Pitch = Pitch < -89.0f ? -89.0f : Pitch;
        }

        updateCameraVectors();
    }

    void ProcessMouseScroll(float yoffset)
    {
        if (Zoom >= 1.0f && Zoom <= 45.0f)
            Zoom -= yoffset;
        Zoom = Zoom <= 1.0f ? 1.0f : Zoom;
        Zoom = Zoom >= 45.0f ? 45.0f : Zoom;
    }

private:
    void updateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);

        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }
};
