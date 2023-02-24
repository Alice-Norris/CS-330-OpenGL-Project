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

#include "Shader.h"

#include <fstream>
#include <iostream>

// Shader constructor. Takes in a vertex and fragment shader source file,
// generates an id, and creates a program from vert and frag sources.
Shader::Shader(std::string vertSrcFile, std::string fragSrcFile) {
  // Creating program, getting shader id
  id = glCreateProgram();

  // Generating vertex and fragment shader IDs
  GLuint vertId = glCreateShader(GL_VERTEX_SHADER);
  GLuint fragId = glCreateShader(GL_FRAGMENT_SHADER);

  // array to hold src c string
  char* src = new char[8192];

  // holds number of characters read from ifstream
  GLint charCnt = 0;

  // Input file stream for source
  std::ifstream src_file;

  // open vertex source file stream, read chars and char count, close stream.
  src_file.open("shader/" + vertSrcFile);
  src_file.get(src, 8192, '\0');
  charCnt = static_cast<int>(src_file.gcount());
  src_file.close();

  // Loading vertex shader source, compiling, and attaching shader.
  glShaderSource(vertId, 1, &src, &charCnt);
  glCompileShader(vertId);
  glAttachShader(id, vertId);

  // Open fragment source file stream, read chars and char count, close stream.
  src_file.open("shader/" + fragSrcFile);
  src_file.get(src, 8192, '\0');
  charCnt = static_cast<int>(src_file.gcount());
  src_file.close();

  // Loading fragment shader source, compiling, and attaching shader.
  glShaderSource(fragId, 1, &src, &charCnt);
  glCompileShader(fragId);
  glAttachShader(id, fragId);

  // Link program
  glLinkProgram(id);

  // Print info log if there is a problem
  PrintInfoLog(id, PROGRAM);

  // Delete shaders (the program has them now)
  glDeleteShader(vertId);
  glDeleteShader(fragId);
}

// Called to use the shader program represented by the object.
void Shader::Use() {
  glUseProgram(id);
}

// Loads integers into shader
void Shader::LoadInt(int val, std::string uName) {
  // Get location of variable
  GLint valLoc = glGetUniformLocation(id, uName.c_str());
  // OpenGL returns -1 if uniform not found, exit.
  if (valLoc == -1) {
    std::cerr << "Uniform not found: " << uName << std::endl;
    return;
  }
  // Load integer into shader
  glUniform1i(valLoc, val);
}

// Loads floats into shader
void Shader::LoadFloat(float val, std::string uName) {
  // Get location of variable
  GLint valLoc = glGetUniformLocation(id, uName.c_str());
  // OpenGL returns -1 if uniform not found, exit.
  if (valLoc == -1) {
    std::cerr << "Uniform not found: " << uName << std::endl;
    return;
  }
  // Load float into shader
  glUniform1f(valLoc, val);
}

void PrintInfoLog(GLuint obj, int type) {
  // Enum for readability
  enum ShaderTypes {
    VERTEX = 0x8B31,
    FRAGMENT = 0x8B30
  };

  // Char array for infoLog, integers for length of info log and shader type.
  char* infoLog = NULL;
  GLint infoLogLength = -1;
  GLint shaderType = 0;


  if (type == SHADER) {
    // if it's a shader, we need the type and log length
    glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infoLogLength);
    glGetShaderiv(obj, GL_SHADER_TYPE, &shaderType);
  } else if (type == PROGRAM) {
    // if it's a program we just need the info log length
    glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infoLogLength);
  }

  // Checking if info log is empty
  if (infoLogLength > 0) {
    // if not, get the log.
    infoLog = new char[infoLogLength];

    // For a shader we can print which stage has the problem.
    if (type == SHADER) {
      glGetShaderInfoLog(obj, infoLogLength, NULL, infoLog);
      if (shaderType == VERTEX) {
        std::cerr << "VERTEX SHADER INFO: ";
      } else if (shaderType == FRAGMENT) {
        std::cerr << "FRAGMENT SHADER INFO: ";
      }
    } else if (type == PROGRAM) {  // Programs just have the one
      glGetProgramInfoLog(obj, infoLogLength, NULL, infoLog);
      std::cerr << "PROGRAM INFO: ";
    }
    // Print infolog.
    std::cerr << infoLog << std::endl;
  }
}
