// Alice Norris, SNHU, Project 1 of CS-330 23EW3
// All code follows Google's style guide as closely as possible
// without sacrificing clarity. Excessive comments are meant to
// demonstrate understanding and not to explain the obvious.

// This file contains the struct definitions for the two types
// of lights implemented (DirLight and PntLight, as well as the functions to
// load light data from a vector of light definitions.

#include "Lights.h"
#include <string>
#include <vector>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

// Directional Light structs
std::vector<DirLight> dirLights{
  DirLight{
    glm::vec4(0.0f, -0.8321f, -0.5547f, 0.0f),  // direction
    glm::vec4(1.0f, 1.0f, 1.0f, 0.0f),          // diffuse
    glm::vec4(0.0f, 1.0f, 1.0f, 0.0f),          // specular
    glm::vec4(1.0f, 1.0f, 1.0f, 0.0f),          // ambient
    0.3f                                        // intensity
  }
};

// Point light structs
std::vector<PntLight> pntLights{
  PntLight{
    glm::vec4(5.0f, 15.0f, -5.0f, 0.0f),  // position
    glm::vec4(1.0f, 0.2f, 0.8f, 0.0f),    // ambient
    glm::vec4(0.8f, 0.2f, 0.8f, 0.0f),    // diffuse
    glm::vec4(0.6f, 0.2f, 1.0f, 0.0f),    // specular
    1.0f,                                 // linear value
    0.045f,                               // constant value
    0.0075f,                              // quadratic value
    1.0f                                  // intensity
  }
};

// Loads directional light data into a buffer so that we can change
// Shader programs without having to reload the light data.
void LoadDirLights(GLFWwindow* window) {
  // Getting pointer array from window and getting shader pointers from it.
  void** objArrPtr = reinterpret_cast<void**>(glfwGetWindowUserPointer(window));
  Shader* imgMatShader = reinterpret_cast<Shader*>(objArrPtr[2]);
  Shader* propMatShader = reinterpret_cast<Shader*>(objArrPtr[3]);

  // Directional light UBO
  GLuint dirLightUBO = 0;

  // Getting block indices, one for each shader
  GLuint propMatDirLightIndex = glGetUniformBlockIndex(propMatShader->id,
                                                       "DirLights");
  GLuint imgMatDirLightIndex = glGetUniformBlockIndex(imgMatShader->id,
                                                      "DirLights");

  // Binding property- and image- material shaders to uniform binding points
  glUniformBlockBinding(propMatShader->id, propMatDirLightIndex, 0);
  glUniformBlockBinding(imgMatShader->id, imgMatDirLightIndex, 0);

  // Generating buffers and setting their sizes (note data is NULL)
  glGenBuffers(1, &dirLightUBO);
  glBindBuffer(GL_UNIFORM_BUFFER, dirLightUBO);
  glBufferData(GL_UNIFORM_BUFFER, (8 * 80) + 4, NULL, GL_STATIC_DRAW);

  // Bind buffer to load directional light data
  glBindBuffer(GL_UNIFORM_BUFFER, dirLightUBO);

  // Maximum of eight directional lights in scene, loop through all 8.
  for (int i = 0; i < 8; ++i) {
    // Size of a directional light, used for loading buffer (68)
    GLint dirLightSz = sizeof(DirLight);
    // If an ith dir light exists, load it into the buffer
    if (i < dirLights.size()) {
      // Load dir light data, starting at byte 0 of ith 80-byte block.
      glBufferSubData(GL_UNIFORM_BUFFER, i * 80, dirLightSz, &dirLights.at(i));
      // Last dirlight member is just a flaot, but still padded to 16 bytes
      // due to std140 layout in shader. Finishes filling data at end.
      glBufferSubData(GL_UNIFORM_BUFFER, i * 65, 3, 0);
    } else {
      // If an ith dir light does not exist, load all zeroes into that block
      glBufferSubData(GL_UNIFORM_BUFFER, i * 80, dirLightSz, 0);
    }
  }
  // Getting number of dir lights and loading it into the buffer
  int dirLightsSz = dirLights.size();
  glBufferSubData(GL_UNIFORM_BUFFER, 640, 4, &dirLightsSz);

  // Unbinding buffer
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  // Defining range of buffer that links to a uniform binding point
  // (the whole thing)
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, dirLightUBO);
}

// Loads point light data into a buffer so that we can change
// Shader programs without having to reload the light data.
void LoadPntLights(GLFWwindow* window) {
  // Getting pointer array from window and getting shader pointers from it.
  void** objArr = reinterpret_cast<void**>(glfwGetWindowUserPointer(window));
  Shader* imgMatShader = reinterpret_cast<Shader*>(objArr[2]);
  Shader* propMatShader = reinterpret_cast<Shader*>(objArr[3]);

  // Point light UBO
  GLuint pntLightUBO = 0;

  // Getting block indices, one for each shader
  GLuint propMatPntLightIndex = glGetUniformBlockIndex(propMatShader->id,
                                                       "PntLights");
  GLuint imgMatPntLightIndex = glGetUniformBlockIndex(imgMatShader->id,
                                                      "PntLights");

  // Binding property- and image- material shaders to uniform binding points
  glUniformBlockBinding(propMatShader->id, propMatPntLightIndex, 1);
  glUniformBlockBinding(imgMatShader->id, imgMatPntLightIndex, 1);

  // Generating buffers and setting their sizes (note data is NULL)
  glGenBuffers(1, &pntLightUBO);
  glBindBuffer(GL_UNIFORM_BUFFER, pntLightUBO);
  glBufferData(GL_UNIFORM_BUFFER, (8 * 80) + 4, NULL, GL_STATIC_DRAW);

  // Maximum of eight point lights in scene, loop through all 8.
  for (int i = 0; i < 8; ++i) {
    // Size of a point light, used for loading buffer (80)
    GLint pntLightSz = sizeof(PntLight);
    if (i < pntLights.size()) {
      // Load pnt light data, starting at byte 0 of ith 80-byte block.
      glBufferSubData(GL_UNIFORM_BUFFER, i * 80, pntLightSz, &pntLights.at(i));
    } else {
      // If an ith pnt light does not exist, load all zeroes into block.
      glBufferSubData(GL_UNIFORM_BUFFER, i * 80, pntLightSz, 0);
    }
  }
  // Getting number of point lights and loading it into buffer
  int numPntLights = pntLights.size();
  glBufferSubData(GL_UNIFORM_BUFFER, 640, 4, &numPntLights);

  // Defining range of buffer that links to a uniform binding point
  // (the entire buffer)
  glBindBufferBase(GL_UNIFORM_BUFFER, 1, pntLightUBO);
}
