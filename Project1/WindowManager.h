#pragma once

#ifndef WIN_MGR
// Alice Norris, SNHU, Project 1 of CS-330 23EW3
// All code follows Google's style guide as closely as possible
// without sacrificing clarity. Excessive comments are meant to
// demonstrate understanding and not to explain the obvious.

// The Window Manager class holds attributes about the OpenGL window, as well
// as a window pointer. The window itself is created with initWindow, which 
// creates the window and sets up input, resize, and debug callbacks. Construct
// with dbgMode set to true for debug messages each frame.
#define WIN_MGR
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>

#include <glm/glm.hpp>


// Window manager. Handles callbacks, input, and of course drawing the window.
class WindowManager {
 private:
    // True to enable debug output, prints logs.(Debug output is awful for
    // performance due to the pretty printing function I wrote!)
    bool dbgMode = false;

    // window attributes
    GLfloat windowHeight = 0;
    GLfloat windowWidth = 0;
    std::string windowTitle = "None";

    // window pointer
    GLFWwindow* window = nullptr;

    // Sets up and creates window. Initializes GLFW and GLW
    void initWindow();

    // Window and Input Callbacks
    // Detects mouse movement, controls camera
    static void mouseMoveCallback(GLFWwindow* window, double x, double y);

    // Detects scroll wheel input, adjusts camera fly speed
    static void mouseScrollCallback(GLFWwindow* window,
                                    double xOffset, double yOffset);

    // Resize callback, makes viewport match window
    static void resizeCallback(GLFWwindow* window, int width, int height);

    // Debug output callback
    static void dbgMsgCallback(GLenum source, GLenum type, GLuint id,
                               GLenum sev, GLsizei len, const GLchar* msg,
                               const void* userParam);

 public:
    // Window manager ctor. Parameters required to make window
    WindowManager(GLfloat windowHeight, GLfloat windowWidth,
                  std::string title, bool dbgMode);

    // Cleanup and close window
    int Terminate();

    // Check whether window ought to close
    bool closeCheck();

    // Processes keyboard input. WASDQE to move, Esc to exit.
    void ProcessInput();

    // Returns window pointer
    GLFWwindow* GetWinPtr();
};
#endif
