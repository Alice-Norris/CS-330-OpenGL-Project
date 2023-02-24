// Alice Norris, SNHU, Project 1 of CS-330 23EW3
// All code follows Google's style guide as closely as possible
// without sacrificing clarity. Excessive comments are meant to
// demonstrate understanding and not to explain the obvious.

// The camera class allows navigation around the 3D world. Movement is
// controlled by keyboard. W/S for backward/forward, A/D for left/right,
// and Q/E for up down. Camera speed is controlled by the mouse scroll wheel.
// Camera yaw and pitch are controlled by mouse movement. P key toggles between
// orthographic and perspective projections.

#include "Camera.h"
#include "Shader.h"

Camera::Camera(glm::vec3 pos, glm::vec3 tar, GLfloat width, GLfloat height) {
  // id of buffer for camera data
  camDataUBO = 0;

  // Time variables
  time = static_cast<float>(glfwGetTime());  // Last time value
  deltaTime = 0;                             // Time since last frame
  projSwTime = 0.0f;                         // Last projection switch time.

  //  Camera Booleans
  firstLook = true;   // (Tries to) prevent weirdness on first move.
  ortho = false;      // Determines whether to load ortho or perspective proj.

  //  Camera Movement Speed Variables
  minSpeed = 2.50f;
  maxSpeed = 5.00f;
  camSpeed = 3.00f;  // Default camera move speed

  //  View Direction Variables
  lookSens = 0.1f;     //  How much mouse movement affects view movement
  yaw = -90.0f;        //  Default yaw angle, negative due to convention
  pitch = 0.0f;        //  Default pitch angle
  lastX = width / 2;   // Default coordinates: center of screen
  lastY = height / 2;  // Default coordinates: center of screen


  //  Camera Vectors
  camVecs = new glm::vec3[4]{
    glm::vec3(0.0f, 1.0f, 0.0f),   // RIGHT
    glm::vec3(0.0f, 0.0f, -1.0f),  // FRONT
    glm::vec3(0.0f, 1.0f, 0.0f),   // UP
    pos                            // POSITION
  };

  // Calculating view matrix
  view = glm::lookAt(camVecs[POSITION],
                     camVecs[POSITION] + camVecs[FRONT],
                     camVecs[UP]);

  // Calculating perspective and orthographig projections
  float aspectRatio = width / height;
  perspProj = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 150.0f);
  orthoProj = glm::ortho(-30.00f, 30.00f, -20.00f, 20.00f, 0.1f, 100.0f);
}

// Recalculates and reloads view matrix into camera data buffer
void Camera::updateViewMat() {
  // Recalculating new view matrix
  view = glm::lookAt(camVecs[POSITION],
                     camVecs[POSITION] + camVecs[FRONT],
                     camVecs[UP]);

  // Bind UBO for camera data
  glBindBuffer(GL_UNIFORM_BUFFER, camDataUBO);

  // Load view matrix (64 bytes) into buffer, starting at byte 0.
  glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, glm::value_ptr(view));

  // Unbind camera data ubo.
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

// Generates buffer, binds buffers and uniform blocks to a ubo, and loads
// initial camera data
void Camera::BindCamData(GLFWwindow* window) {
  void** objArr = reinterpret_cast<void**>(glfwGetWindowUserPointer(window));
  Shader* imgMatShader = reinterpret_cast<Shader*>(objArr[2]);
  Shader* propMatShader = reinterpret_cast<Shader*>(objArr[3]);

  // Get block indices for both shaders' camera data uniform blocks
  GLuint iCamDataIndex = glGetUniformBlockIndex(imgMatShader->id, "camData");
  GLuint pCamDataIndex = glGetUniformBlockIndex(propMatShader->id, "camData");

  // Binding uniform blocks to index 2 (0 and 1 are taken by the light UBOs)
  glUniformBlockBinding(imgMatShader->id, iCamDataIndex, 2);
  glUniformBlockBinding(propMatShader->id, pCamDataIndex, 2);

  // Generating buffer for UBO and binding
  glGenBuffers(1, &camDataUBO);
  glBindBuffer(GL_UNIFORM_BUFFER, camDataUBO);

  // Create buffer of 144 bytes, filled with NULL
  // 16 floats (view mat) +
  // 16 floats (proj mat) +
  //  4 floats (cam pos) = 36 * 4 (bytes/float) 144 bytes
  glBufferData(GL_UNIFORM_BUFFER, 144, NULL, GL_STATIC_DRAW);

  // Load view matrix data (64 bytes) into buffer starting at byte 0
  glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, glm::value_ptr(view));

  // Loading projection matrix depending on whether ortho is on or not.
  // Loads projection matrix data (64 bytes) into buffer starting at byte 64
  if (ortho) {
    glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, glm::value_ptr(orthoProj));
  } else {
    glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, glm::value_ptr(perspProj));
  }

  // Get pointer to camera position, pack to vec4, and
  // Load camera position into buffer (16 bytes) into buffer starting at 128.
  const float* camPosPtr = glm::value_ptr(glm::vec4(camVecs[POSITION], 0.0f));
  glBufferSubData(GL_UNIFORM_BUFFER, 128, 16, camPosPtr);

  // Unbind buffer
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  // Bind entire UBO to binding point 2.
  glBindBufferBase(GL_UNIFORM_BUFFER, 2, camDataUBO);
}

// Updates camera view direction when called.
void Camera::updateDir(float x, float y) {
  // Check if camera has moved before, exit if so (preventing jumping)
  if (firstLook) {
    firstLook = false;
    return;
  }

  // Updating yaw and pitch based on change in cursor position
  yaw += lookSens * (x - lastX);
  pitch += lookSens * (lastY - y);

  // Store last x and y.
  lastX = x;
  lastY = y;

  // Limit pitch to <= straight up and >= straight down
  if (pitch > 89.0f) {
    pitch = 89.0f;
  }
  if (pitch < -90.0f) {
    pitch = -90.0f;
  }

  // Calculating new fromt vector
  glm::vec3 newFront = glm::vec3(
    cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
    sin(glm::radians(pitch)),
    sin(glm::radians(yaw)) * cos(glm::radians(pitch)));

  // Updating camera directional vectors
  camVecs[FRONT] = glm::normalize(newFront);
  glm::vec3 rightVec = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), camVecs[FRONT]);
  camVecs[RIGHT] = glm::normalize(rightVec);
  camVecs[UP] = glm::normalize(glm::cross(camVecs[FRONT], camVecs[RIGHT]));

  // Update view matrix since view vectors changed
  updateViewMat();
}

// Updates camera position when W/A/S/D/Q/E input is detected
void Camera::updatePos(unsigned int moveBits) {
  // Calculating distance moved with dist = vel * time
  float moveDist = camSpeed * deltaTime;

  // calculating offset by multiplying move distance by directional vectors.
  // If a bit is not set, move distance is multiplied by zero,
  // nullifying it. This allows the camera to move along combined
  // directions.
  glm::vec3 offset =
    (1 & moveBits) * moveDist * camVecs[FRONT] +
    ((2 & moveBits) / 2) * -moveDist * camVecs[FRONT] +
    ((4 & moveBits) / 4) * moveDist * camVecs[RIGHT] +
    ((8 & moveBits) / 8) * -moveDist * camVecs[RIGHT] +
    ((16 & moveBits) / 16) * moveDist * globalY +
    ((32 & moveBits) / 32) * -moveDist * globalY;

  camVecs[POSITION] += offset;
  // Bind camera data buffer
  glBindBuffer(GL_UNIFORM_BUFFER, camDataUBO);

  // Get pointer to camera position, pack to a vec4, and update
  // the camera view direction, loading vec4 (16 bytes) starting at byte 128.
  const float* camPosPtr = glm::value_ptr(glm::vec4(camVecs[POSITION], 0.0f));
  glBufferSubData(GL_UNIFORM_BUFFER, 128, 16, camPosPtr);

  // Unbind buffer
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  // Update view matrix since camera position changed
  updateViewMat();
}


// Update's camera's time and time since last frame when called
void Camera::updateTime() {
  // get current time
  float curr_time = static_cast<float>(glfwGetTime());

  // Updating time since last frame
  deltaTime = curr_time - time;

  // Updating current time
  time = curr_time;
}


// Adjusts camera speed when scroll input is detected
void Camera::adjustSpeed(float xOffset, float yOffset) {
  // if scroll down, decrease speed. Scrool up, increase speed.
  if (yOffset == -1 && camSpeed > minSpeed) {
    camSpeed -= 0.1f;
  } else if (yOffset == 1 && camSpeed < maxSpeed) {
    camSpeed += 0.1f;
  }
}

// Switches between projection and orthographic matrices
void Camera::projSwitch() {
  // Time limit here to fix flickering (from reading multiple keystrokes/frame)
  if (static_cast<float>(glfwGetTime()) - projSwTime > 0.05) {
    // Toggle ortho boolean
    ortho = !ortho;

    // Bind cam data buffer
    glBindBuffer(GL_UNIFORM_BUFFER, camDataUBO);

    // Check new ortho value and load appropriate projection matrix (64 bytes)
    // starting at position 64.
    if (ortho) {
      glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, glm::value_ptr(orthoProj));
    } else {
      glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, glm::value_ptr(perspProj));
    }
    // Unbind buffer
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
  }

  // Reset time since last projection switch
  projSwTime = static_cast<float>(glfwGetTime());
}

