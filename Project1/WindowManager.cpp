// Alice Norris, SNHU, Project 1 of CS-330 23EW3
// All code follows Google's style guide as closely as possible
// without sacrificing clarity. Excessive comments are meant to
// demonstrate understanding and not to explain the obvious.

// The Window Manager class holds attributes about the OpenGL window, as well
// as a window pointer. The window itself is created with initWindow, which 
// creates the window and sets up input, resize, and debug callbacks. Construct
// with dbgMode set to true for debug messages each frame.
#include "WindowManager.h"
#include "ModelManager.h"

#include <map>
#include <iomanip>

#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"

// Window manager ctor. All parameters necessary to make a window.
WindowManager::WindowManager(GLfloat height, GLfloat width,
                             std::string title, bool dbgMode) {
  windowHeight = height;
  windowWidth = width;
  windowTitle = title;
  initWindow();
}

// Creates the actual window
void WindowManager::initWindow() {
  // Initialize GLFW
  glfwInit();

  // Setting opengl version and etc. for window
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  // Creating window
  window = glfwCreateWindow(
    static_cast<int>(windowHeight), static_cast<int>(windowWidth),
    windowTitle.c_str(), NULL, NULL);

  // Make context current on this thread
  glfwMakeContextCurrent(window);

  // Setting resize callback and viewport size
  glfwSetFramebufferSizeCallback(window, resizeCallback);

  // Modifying how window handles mouse, setting window callbacks,
  // and centering cursor.
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPos(window, windowWidth / 2, windowHeight / 2);
  glfwSetCursorPosCallback(window, mouseMoveCallback);
  glfwSetScrollCallback(window, mouseScrollCallback);

  // Setting viewport size and options
  glViewport(0, 0, 800, 600);
  glEnable(GL_DEPTH_TEST);

  // If debug mode is enabled, debug output is turned on and debug callback
  // registered.
  if (dbgMode) {
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(&dbgMsgCallback, NULL);
  }
  // Initialize GLEW now that the window is current to the OpenGL context.
  glewInit();
}

// Callback for window resizes
void WindowManager::resizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

// Called on mouse scroll, adjusts camera fly speed
void WindowManager::mouseScrollCallback(GLFWwindow* window,
                                        double xOffset, double yOffset) {
  // Getting camera pointer from object array
  Camera* camPtr = reinterpret_cast<Camera*>(
                     reinterpret_cast<void**>(
                       glfwGetWindowUserPointer(window))[4]);

  camPtr->adjustSpeed(static_cast<float>(xOffset), static_cast<float>(yOffset));
}

// Called when a mouse move is detected, updates camera pitch and yaw
void WindowManager::mouseMoveCallback(GLFWwindow* window,
                                      double x, double y) {
  // Getting camera pointer from object array
  Camera* camPtr = reinterpret_cast<Camera*>(
                     reinterpret_cast<void**>(
                       glfwGetWindowUserPointer(window))[4]);

  camPtr->updateDir(static_cast<float>(x), static_cast<float>(y));
}

// Debug printing callback. BAD FOR PERFORMANCE
void WindowManager::dbgMsgCallback(GLenum source, GLenum type, GLuint id,
                                   GLenum sev, GLsizei len, const GLchar* msg,
                                   const void* userParam) {
    // Turning OpenGL constants back into strings
    std::map<unsigned int, std::string> dbgSrcMap{
        {0x8246, "OpenGL API Call"},
        {0x8247, "Window System Call"},
        {0x8248, "GLSL Compiler"},
        {0x824B, "Other"}
    };

    std::map<unsigned int, std::string> dbgTypMap{
        {0x824C, "API Error"},
        {0x824D, "Deprecated"},
        {0x824E, "Undefined"},
        {0x824F, "Not Portable"},
        {0x8251, "Other"},
    };

    std::map<unsigned int, std::string> dbgSevMap{
        {0x9146, "High"},
        {0x9148, "Medium"},
        {0x9148, "Low"},
        {0x826B, "Notification"},
    };

    // Print formatted debug output
    std::cout << std::setw(20) << std::left << std::setfill('-') << "-"
              << std::setw(0) << "DEBUG MSG"
              << std::setw(19) << std::right << " -\n";

    std::cout << std::setw(22) << std::left << std::setfill(' ') << "| Source:"
              << std::setw(23) << std::right << dbgSrcMap[source]
              << " |\n";

    std::cout << std::setw(22) << std::left << "| Type:"
              << std::setw(23) << std::right << dbgTypMap[type]
              << " |\n";

    std::cout << std::setw(22) << std::left << "| ID:"
              << std::setw(23) << std::right << id << " |\n";

    std::cout << std::setw(22) << std::left << "| Severity:"
              << std::setw(23) << std::right << dbgSevMap[sev]
              << " |\n";

    std::cout << std::setw(24) << std::left << "| Message:"
              << std::setw(24) << std::right << " |\n";

    // convert message to string for processing
    std::string msgStr(msg);

    // print message in 42 character segments with formatting
    int index = 0;
    int msgLen = len;
    while (msgLen > 0) {
        std::cout << std::left << "| " << std::setw(42)
                  << msgStr.substr(index * 42, 42)
                  << "  |\n";
        msgLen -= 42;
        ++index;
    }
    // Print message footer
    std::cout << "-" << std::setw(45) << std::setfill('-') << "" << "-\n\n";

    // Place breakpoint here to stop on all logs other than notifications
    if (sev == GL_DEBUG_SEVERITY_HIGH ||
        sev == GL_DEBUG_SEVERITY_LOW  ||
        sev == GL_DEBUG_SEVERITY_MEDIUM) {
        GLenum e = glGetError();
    }
}

// Simply returns whether the window should close from glfw
bool WindowManager::closeCheck() {
  return glfwWindowShouldClose(window);
}

// Returns window pointer.
GLFWwindow* WindowManager::GetWinPtr() {
  return window;
}

// Processes user input. Keys:
// W/S : forward/back
// A/D : left/right
// Q/E : up/down
// P   : switch perspective
// Esc : exit program

void WindowManager::ProcessInput() {
  void** objArr = reinterpret_cast<void**>(glfwGetWindowUserPointer(window));
  Camera* camPtr = reinterpret_cast<Camera*>(objArr[4]);
  unsigned int moveBits = 0;
  // Move on WASD and Q/E keys. W and S are forward and reverse,
    // A and D are left and right, Q and E are up and down.
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    moveBits ^= 0b000001;
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    moveBits ^= 0b000010;
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    moveBits ^= 0b000100;
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    moveBits ^= 0b001000;
  }
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
    moveBits ^= 0b010000;
  }
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
    moveBits ^= 0b100000;
  }
  if (moveBits != 0) {
    camPtr->updatePos(moveBits);
  }
  // Set whether window should close. Exits program.
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
  // Toggle ortho/persp projection
  if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
    camPtr->projSwitch();
  }
}
