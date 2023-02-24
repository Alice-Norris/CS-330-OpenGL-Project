// Alice Norris, SNHU, Project 1 of CS-330 23EW3
// All code follows Google's style guide as closely as possible
// without sacrificing clarity. Excessive comments are meant to
// demonstrate understanding and not to explain the obvious.

// The Model Manager class creates, loads, and stores data on meshes, textures,
// materials, and models. This class also draws the models once all setup has 
// been completed. All products are held privately and inaccessible to prevent
// the temptation to manipulate them directly.
#pragma once

#ifndef MOD_MGR
#define MOD_MGR

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <assimp/Importer.hpp>

#include <glm/glm.hpp>

#include "Shader.h"
#include "Camera.h"

// Model manager: Creates and stores models, materials, textures, and meshes.
// Also loads texture images and mesh data into OpenGL context.
class ModelManager {
 private:
  enum ObjType{
    WINMGR,
    MODMGR,
    IMGSHDR,
    PROPSHDR,
    SCENECAM
  };

  // Materials. Used for models that do not have an image texture.
  // Model Component.
  struct Material {
    glm::vec3 amb = glm::vec3(0.0f);
    glm::vec3 diff = glm::vec3(0.0f);
    glm::vec3 spec = glm::vec3(0.0f);
    float gloss = 0.0f;
  };

  // Textures. Used for models with image textures.
  // Model Component.
  struct Texture {
    GLuint diffTex = 0;
    GLuint specTex = 0;
    float gloss = 0.0f;
  };

  // Vertex data. Holds 8 floats, 3 for position, 3 for normals, 2 for textures.
  // Mesh component.
  struct Vertex {
    glm::vec3 pos = glm::vec3(0.0f);
    glm::vec3 norm = glm::vec3(0.0f);
    glm::vec2 uv = glm::vec3(0.0f);
    friend std::ostream& operator<<(std::ostream& out, const Vertex& vert);
  };


  // Mesh data. Holds Vertex Array and Buffer IDs,
  // as well as vertex and index data. Model component.
  struct Mesh {
    GLuint VBO = 0;
    GLuint VAO = 0;
    GLuint EBO = 0;
    std::vector<Vertex> verts;
    std::vector<GLuint> indices;
  };

  // Final Product of model manager. Has the name of its mesh and material
  // (image- or property- based) and its matrices.
  struct Model {
    std::string meshName = "";
    std::string matName = "";
    glm::mat4 modelMat = glm::mat4(1.0f);
    glm::mat3 normMat = glm::mat3(1.0f);
  };

  // Storage maps for materials, textures, meshes, and models
  std::map<std::string, Material> materials;
  std::map<std::string, Texture> textures;
  std::map<std::string, Mesh> meshes;
  std::map<std::string, Model> models;

  // Asset Importer (Reads mesh data from file in ReadMesh)
  Assimp::Importer importer;

  // Reads mesh data from file, using a filename and a pointer to a mesh.
  // Must be DAE format. Called by CreateMeshes.
  void ReadMesh(std::string filename, Mesh* mesh);

  // Loads mesh data into OpenGL context using a mesh struct.
  // Called by CreateMeshes after ReadMesh
  void LoadMesh(Mesh* mesh);

  // Loads texture image into OpenGL context using a filename and
  // texture ID. Called by CreateTextures.
  void LoadTexture(std::string filename, GLuint* texIdPtr);

  // Draws the model.
  void DrawModel(std::string modelName, GLFWwindow* window);

 public:
  // Material definition, for use in CreateMaterials function by program.
  struct MaterialDef {
    std::string matName = "";
    glm::vec3 amb = glm::vec3(0.0f);
    glm::vec3 diff = glm::vec3(0.0f);
    glm::vec3 spec = glm::vec3(0.0f);
    float gloss = 0.0f;
  };

  // Material definition, for use in CreateTextures function by program.
  struct TextureDef {
    std::string texName = "";
    std::string diffTexFile = "";
    std::string specTexFile = "";
    float gloss = 0.0f;
  };

  // Model definition, for use in ModelTextures function by program.
  struct ModelDef {
    std::string modelName = "";
    std::string meshName = "";
    std::string matName = "";
    glm::mat4 modelMat = glm::mat4(1.0f);
  };

  // These functions are meant to be used by the program to load model data.
  // CreateModels should be called only after all materials, textures, and
  // meshes have been created. DrawModels should only be called after all
  // CreateModel calls are complete.
  void CreateMaterials(std::vector<MaterialDef> matDefs);
  void CreateTextures(std::vector<TextureDef> texDefs);
  void CreateMeshes(std::vector<std::string> filenames);
  void CreateModels(std::vector<ModelDef> modDefs);
  void DrawModels(GLFWwindow* window);
};
#endif
