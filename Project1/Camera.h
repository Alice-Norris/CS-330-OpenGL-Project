// Alice Norris, SNHU, Project 1 of CS-330 23EW3
// All code follows Google's style guide as closely as possible
// without sacrificing clarity. Excessive comments are meant to
// demonstrate understanding and not to explain the obvious.

// The camera class allows navigation around the 3D world. Movement is
// controlled by keyboard. W/S for backward/forward, A/D for left/right,
// and Q/E for up down. Camera speed is controlled by the mouse scroll wheel.
// Camera yaw and pitch are controlled by mouse movement. P key toggles between
// orthographic and perspective projections.
#pragma once
#ifndef CAM
#define CAM

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Camera {
 private:
    // Direction of global Y axis (Unit vector)
    glm::vec3 globalY = glm::vec3(0.0f, 1.0f, 0.0f);
    // Vector indices for positional vectors
    enum vecs {
      RIGHT = 0,
      FRONT = 1,
      UP = 2,
      POSITION = 3
    };

    // Buffer IDs
    GLuint camDataUBO;  // Camera data buffer ID

    // Time variables
    float time;               // last time value
    float deltaTime;          // change in time since last frame
    float projSwTime;  // time since last projection switch

    // Camera Booleans
    bool firstLook;  // tracks first move of mouse
    bool ortho;      // determines which perspective is loaded

    // Camera Movement Speed Variables
    float minSpeed;
    float maxSpeed;
    float camSpeed;

    // View Direction Variables
    float lookSens;  // Look sensitivity for mouse movement
    float yaw;       // Left/Right look angle
    float pitch;     // Up/Down look angle
    float lastX;     // Last X coordinate of mouse cursor
    float lastY;     // Last Y coordinate of mouse cursor

    // Camera Vectors
    glm::vec3* camVecs;  // RIGHT, FRONT, UP, and POSITION vectors.

    // Camera Matrices
    glm::mat4 view;       // View matrix of camera
    glm::mat4 perspProj;  // perspective projection
    glm::mat4 orthoProj;  // orthographig projection


    // Updates view matrix when vectors change
    void updateViewMat();

 public:
    // Camera constructor
    Camera(glm::vec3 pos, glm::vec3 tar, GLfloat width, GLfloat height);

    // Enum for movement commands
    enum moveDir {
      NEG_Z = 1,
      POS_Z = 2,
      NEG_X = 4,
      POS_X = 8,
      NEG_Y = 16,
      POS_Y = 32,
    };

    // Generates buffers, binds UBO to shader and buffer binding points,
    // Loads intial camera data.
    void BindCamData(GLFWwindow* window);

    // Updates camera look direction on mouse movement
    void updateDir(float xOffset, float yOffset);

    // Update camera position on W/A/S/D/Q/E input
    void updatePos(unsigned int moveBits);

    // Updates camera time
    void updateTime();

    // Adjusts camera speed when scroll wheel is moved
    void adjustSpeed(float xOffset, float yOffset);

    // Switches which projection matrix is loaded by the camera
    void projSwitch();
};
#endif
