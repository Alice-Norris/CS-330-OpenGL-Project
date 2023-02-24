// Alice Norris, SNHU, Project 1 of CS-330 23EW3
// All code follows Google's style guide as closely as possible
// without sacrificing clarity. Excessive comments are meant to
// demonstrate understanding and not to explain the obvious.

// The Shader class creates an object representing a shader program.
// The Shader object is created after loading, compiling, and linking
// the shader program in the opengl context. Each shader also has utility
// functions to load matrices, vectors, and floats into it, as well as Use,
// which sets the shader as the active program in the OpenGL context.
#pragma once
#ifndef SHD_MGR
#define SHD_MGR

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <glm/gtc/type_ptr.hpp>

enum objType {
  SHADER = 0,
  PROGRAM = 1
};

class Shader {
 public:
  GLuint id = 0;  // Shader object's id, given by OpenGL context.

  // Constructor
  Shader(std::string vertSrcFile, std::string fragSrcFile);

  // Called to use the shader program represented by the object
  void Use();

  // function to load single int into shader
  void LoadInt(int val, std::string name);

  // function to load single floats into shader
  void LoadFloat(float val, std::string name);

  // Templated function to take integer vectors of any size 2-4.
  template <int vSz, glm::qualifier Q>
  void LoadVector(struct glm::vec<vSz, float, Q> vector, std::string uName);

  // Templated function to take float vectors of any size 2-4.
  template <int size, glm::qualifier qual>
  void LoadVector(struct glm::vec<size, int, qual> vector, std::string uName);

  // Templated function to take float matrices of any size 2-4.
  template <int col, int row, glm::qualifier qual>
  void LoadMatrix(struct glm::mat<col, row, float, qual> matrix,
                  std::string uName);
};

template <int col, int row, glm::qualifier qual>
void Shader::LoadMatrix(struct glm::mat<col, row, float, qual> matrix,
                        std::string uName) {
  // Get location of matrix
  GLint matLoc = glGetUniformLocation(id, uName.c_str());
  // OpenGL returns -1 if uniform not found, exit
  if (matLoc == -1) {
    std::cout << "Uniform not found: " << uName << std::endl;
    return;
  }

  // Function pointer for load function. Can be: glUniformMatrix2fv,
  // 2x3fv, 2x4fv, 3x2fv, 3fv, 3x4fv, 4x2fv, 4x3fv, or 4fv
  void (*loadFunc)(GLint, GLsizei, GLboolean, const GLfloat*) = nullptr;
  // first level switch based on column size of matrix
  switch (col) {
  case 2:
    // second level switch based on row size of matrix
    switch (row) {
    case 2:
      loadFunc = glUniformMatrix2fv;
      break;
    case 3:
      loadFunc = glUniformMatrix2x3fv;
      break;
    case 4:
      loadFunc = glUniformMatrix2x4fv;
      break;
    default:
      std::cout << "Number of rows must be 2-4." << std::endl;
    }
    break;
  case 3:
    // second level switch based on row size of matrix
    switch (row) {
    case 2:
      loadFunc = glUniformMatrix3x2fv;
      break;
    case 3:
      loadFunc = glUniformMatrix3fv;
      break;
    case 4:
      loadFunc = glUniformMatrix3x4fv;
      break;
    default:
      std::cout << "Number of rows must be 2-4." << std::endl;
    }
    break;
  case 4:
    // second level switch based on row size of matrix
    switch (row) {
    case 2:
      loadFunc = glUniformMatrix4x2fv;
      break;
    case 3:
      loadFunc = glUniformMatrix4x3fv;
      break;
    case 4:
      loadFunc = glUniformMatrix4fv;
      break;
    default:
      std::cout << "Number of rows must be 2-4." << std::endl;
    }
    break;
  default:
    std::cout << "Number of columns must be 2-4." << std::endl;
  }
  // Load matrix using function pointer
  loadFunc(matLoc, 1, GL_FALSE, glm::value_ptr(matrix));

  // Call to use program as current active program
  void Use();
}


template <int vSz, glm::qualifier Q>
void Shader::LoadVector(struct glm::vec<vSz, float, Q> vector,
                        std::string uName) {
  // Get location of vector
  GLint vecLoc = glGetUniformLocation(id, uName.c_str());

  // OpenGL returns -1 if uniform not found, exit
  if (vecLoc == -1) {
    std::cout << "Uniform not found: " << uName << std::endl;
    return;
  }

  // Pointer for load func. Can be: glUniform2fv, glUniform3fv, glUniform4fv
  void (*loadFunc)(GLint, GLsizei, const GLfloat*) = nullptr;

  // Choose the right function based on vec size
  switch (vSz) {
  case 2:
    loadFunc = glUniform2fv;
    break;
  case 3:
    loadFunc = glUniform3fv;
    break;
  case 4:
    loadFunc = glUniform4fv;
    break;
  default:
    // Print error if outside of size range, somehow
    std::cout << "Invalid size! Must be 2-4." << std::endl;
  }
  // Call function using function pointer
  loadFunc(vecLoc, 1, (const GLfloat*)glm::value_ptr(vector));
}

// Templated function to take float vectors of any size 2-4.
template <int size, glm::qualifier qual>
void Shader::LoadVector(struct glm::vec<size, int, qual> vector,
                        std::string uName) {
  // Get location of vector
  GLint vecLoc = glGetUniformLocation(id, uName.c_str());

  // OpenGL returns -1 if uniform not found, exit
  if (vecLoc == -1) {
    std::cout << "Uniform not found." << std::endl;
    return;
  }

  // Pointer for load func. Can be: glUniform2fv, glUniform3fv, glUniform4fvz
  void (*loadFunc)(GLint, GLsizei, const GLint*) = nullptr;

  // Choose the right function based on vec size
  std::cout << size << std::endl;
  switch (size) {
  case 2:
    loadFunc = glUniform2iv;
    break;
  case 3:
    loadFunc = glUniform3iv;
    break;
  case 4:
    loadFunc = glUniform4iv;
    break;
  default:
    // Print error if outside of size range, somehow
    std::cout << "Invalid size! Must be 2-4." << std::endl;
  }
  // Call function using function pointer
  loadFunc(vecLoc, 1, (const GLint*)glm::value_ptr(vector));
}

// Prints info log when called.
void PrintInfoLog(GLuint obj, int type);
#endif
