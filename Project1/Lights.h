// Alice Norris, SNHU, Project 1 of CS-330 23EW3
// All code follows Google's style guide as closely as possible
// without sacrificing clarity. Excessive comments are meant to
// demonstrate understanding and not to explain the obvious.

// This file contains the struct definitions for the two types
// of lights implemented (DirLight and PntLight, as well as the functions to
// load light data from a vector of light definitions.
#pragma once

#ifndef LIGHTS
#define LIGHTS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Shader.h"

// Struct for directional and point lights. Vector attributes are vec4
// instead of vec3 to make working with openGL's std140 layout easier
struct DirLight {
  glm::vec4 dir = glm::vec4(0.0f);  // light direction

  // ambient, diff, and specular colors
  glm::vec4 amb = glm::vec4(0.0f);
  glm::vec4 diff = glm::vec4(0.0f);
  glm::vec4 spec = glm::vec4(0.0f);

  // light strength
  float intensity = 0.0f;
};

struct PntLight {
  // Light's position
  glm::vec4 pos = glm::vec4(0.0f);

  // Ambient, diff, and specular colors
  glm::vec4 amb = glm::vec4(0.0f);
  glm::vec4 diff = glm::vec4(0.0f);
  glm::vec4 spec = glm::vec4(0.0f);

  // Attenuation factors
  float constVal = 0.0f;
  float linVal = 0.0f;
  float quadVal = 0.0f;

  // Light strength
  float intensity = 0.0f;
};

// Loads point lights into UBO
void LoadPntLights(GLFWwindow* window);

// Loads directional lights into UBO
void LoadDirLights(GLFWwindow* window);
#endif
