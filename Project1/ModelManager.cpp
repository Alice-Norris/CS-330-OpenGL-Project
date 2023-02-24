// Alice Norris, SNHU, Project 1 of CS-330 23EW3
// All code follows Google's style guide as closely as possible
// without sacrificing clarity. Excessive comments are meant to
// demonstrate understanding and not to explain the obvious.

// The Model Manager class creates, loads, and stores data on meshes, textures,
// materials, and models. This class also draws the models once all setup has 
// been completed. All products are held privately and inaccessible to prevent
// the temptation to manipulate them directly.
#include "ModelManager.h"
#include "WindowManager.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif

// Creates meshes given a vector of filenames of .DAE files to load
void ModelManager::CreateMeshes(std::vector<std::string> filenames) {
  // Iterator for mesh files
  std::vector<std::string>::iterator fileIter = filenames.begin();

  // Iterate over filename vector to the end
  for (; fileIter != filenames.end(); ++fileIter) {
    // Getting filename, creating blank mesh
    std::string filename = *fileIter;
    Mesh mesh;

    // Read and Load mesh
    ReadMesh(filename, &mesh);
    LoadMesh(&mesh);

    // Store mesh with name
    std::string meshName = filename.substr(0, filename.find("."));
    meshes[meshName] = mesh;
  }
}

// Loads mesh into OpenGL context
void ModelManager::LoadMesh(Mesh* mesh) {
  // Generating Vertex Array and Vertex Buffer
  glGenVertexArrays(1, &mesh->VAO);
  glGenBuffers(1, &mesh->VBO);

  // Binding vertex array so settings are saved
  glBindVertexArray(mesh->VAO);

  // Binding VBO
  glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);

  // Calculating required buffer size for VBO
  // (vec3f * 2 + vec2f * 1 = 8 floats * 4 bytes/float = 32 bytes.)
  GLsizeiptr vboBuffSz = sizeof(Vertex) * mesh->verts.size();

  // Loading vertex data into VBO
  glBufferData(GL_ARRAY_BUFFER, vboBuffSz, mesh->verts.data(), GL_STATIC_DRAW);

  // Generating EBO Buffer
  glGenBuffers(1, &mesh->EBO);

  // Binding EBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);

  // Calculating required buffer size for EBO
  // (4 bytes * num indices)
  GLsizeiptr eboBuffSz = sizeof(GLuint) * mesh->indices.size();

  // Loading index data into EBO
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, eboBuffSz,
               mesh->indices.data(), GL_STATIC_DRAW);

  // Defining Vertex Attrib Arrays
  GLsizei stride = sizeof(GLfloat) * 8;
  // Position Attrib Ptr, 3 floats, location 0
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
                        reinterpret_cast<void*>(0));
  // Normal Attrib Ptr, 3 floats, location 1
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                        reinterpret_cast<void*>(3 * sizeof(GLfloat)));
  // UV attrib ptr, 2 floats, location 2
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride,
                        reinterpret_cast<void*>(6 * sizeof(GLfloat)));

  // Enabling Attrib Arrays
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);

  // Unbind new VAO
  glBindVertexArray(0);
}

// Reads Mesh from file
void ModelManager::ReadMesh(std::string filename, Mesh* mesh) {
  // Processing flags for importing the model
  int processFlags = aiProcess_Triangulate |
    aiProcess_JoinIdenticalVertices        |
    aiProcess_SortByPType;

  // Importing scene from file
  const aiScene* scene = importer.ReadFile(("mesh/" + filename), processFlags);

  // Making sure a scene was actually found
  if (scene == nullptr) {
    std::cerr << importer.GetErrorString() << std::endl;
    return;
  }

  // Each file only has one mesh
  aiMesh* importMesh = scene->mMeshes[0];

  // Recreating vertex data
  for (unsigned int i = 0; i < importMesh->mNumVertices; ++i) {
    // Create new Vertex struct
    Vertex vert;

    // Getting vector for each attribute
    aiVector3D vecPos = importMesh->mVertices[i];
    aiVector3D vecNorm = importMesh->mNormals[i];
    aiVector3D vecUv = importMesh->mTextureCoords[0][i];

    // Assigning values to Vertex struct
    vert.pos = glm::vec3(vecPos[0], vecPos[1], vecPos[2]);
    vert.norm = glm::vec3(vecNorm[0], vecNorm[1], vecNorm[2]);
    vert.uv = glm::vec2(vecUv[0], vecUv[1]);

    // Add vertex to vertex vector
    mesh->verts.push_back(vert);
  }

  // Storing indices for each face
  for (unsigned int i = 0; i < importMesh->mNumFaces; ++i) {
    aiFace face = importMesh->mFaces[i];
    for (int j = 0; j < 3; ++j) {
      mesh->indices.push_back(face.mIndices[j]);
    }
  }
}


// Loads texture and image data into OpenGL context
void ModelManager::LoadTexture(std::string filename, GLuint* texIdPtr) {
  // Generating texture ID
  glGenTextures(1, texIdPtr);

  // Image attributes
  int width = 0;
  int height = 0;
  int numChannels = 0;

  // Flip image vertically and read file data and attributes
  stbi_set_flip_vertically_on_load(1);
  unsigned char* data = stbi_load(("tex/" + filename).c_str(),
                                  &width, &height, &numChannels, 0);

  // STBI makes data NULL if loading error
  if (data == NULL) {
    std::cout << "Could not load image!" << std::endl;
    return;
  }

  // Determining pixel type
  GLenum pixelType = 0;
  if (numChannels == 3) {
    pixelType = GL_RGB;
  } else if (numChannels == 4) {
    pixelType = GL_RGBA;
  }

  // Binding texture and loading image data
  glBindTexture(GL_TEXTURE_2D, *texIdPtr);
  glTexImage2D(GL_TEXTURE_2D, 0, pixelType, width, height, 0, pixelType,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  // Setting texture parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Freeing image data
  stbi_image_free(data);
}

// Draws the supplied model name in the window
void ModelManager::DrawModel(std::string modelName, GLFWwindow* window) {
  // Getting array of objects from window pointer
  void** objArr = reinterpret_cast<void**>(glfwGetWindowUserPointer(window));

  // Getting appropriate model and mesh
  Model model = models[modelName];
  Mesh mesh = meshes[model.meshName];

  // Shader pointer, since we haven't determined which shader yet.
  Shader* shader;

  // This if block chooses appropriate shader for model, uses it, and
  // loads shader-specific variables
  std::map<std::string, Texture>::iterator tex = textures.find(model.matName);
  if (tex != textures.end()) {
    // If so, set Shader pointer to image material shader and use it.
    shader = reinterpret_cast<Shader*>(objArr[IMGSHDR]);
    shader->Use();

    // Setting textures and binding them
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex->second.diffTex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex->second.specTex);

    // Setting textures to appropriate points in shader
    shader->LoadInt(0, "diffSamp");
    shader->LoadInt(1, "specSamp");

    // Loading material glossiness into shader
    shader->LoadFloat(tex->second.gloss, "gloss");
  } else {
    // No texture, use property material shader
    shader = reinterpret_cast<Shader*>(objArr[PROPSHDR]);
    shader->Use();

    // Get pointer to material
    Material* mat = &materials[model.matName];

    // Load material attributes into shader
    shader->LoadVector(mat->amb, "material.amb");
    shader->LoadVector(mat->diff, "material.diff");
    shader->LoadVector(mat->spec, "material.spec");
    shader->LoadFloat(mat->gloss, "material.gloss");
  }

  // Load model and norm matrices
  shader->LoadMatrix(model.modelMat, "modelMat");
  shader->LoadMatrix(model.normMat, "normMat");
  GLfloat test[16];
  GLint normMatLoc = glGetUniformLocation(shader->id, "normMat");
  glGetUniformfv(shader->id, normMatLoc, test);
  // Bind mesh's vertex array
  glBindVertexArray(mesh.VAO);

  WindowManager* winMgrPtr = reinterpret_cast<WindowManager*>(objArr[0]);
  winMgrPtr->ProcessInput();
  // Drawing model
  // WindowManager* winMgrPtr = reinterpret_cast<WindowManager*>(ptrs[0]);
  glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
}

// Creates Materials from material definitions
void ModelManager::CreateMaterials(std::vector<MaterialDef> matDefs) {
  // Iterator for material definitions vector
  std::vector<MaterialDef>::iterator matIter = matDefs.begin();
  // Increment iterator until end, creating material each time
  for (; matIter != matDefs.end(); ++matIter) {
    // Create a new material and store it, using the name as a key
    Material newMat{ matIter->amb,
                     matIter->diff,
                     matIter->spec,
                     matIter->gloss };
    materials[matIter->matName] = newMat;
  }
}

// Creates Textures from texture definitions
void ModelManager::CreateTextures(std::vector<TextureDef> texDefs) {
  // Iterator for texture definitions vector
  std::vector<TextureDef>::iterator texIter = texDefs.begin();
  // Increment iterator until end, creating texture each time
  for (; texIter != texDefs.end(); ++texIter) {
    // Create new texture
    Texture newTex;

    // Load texture's images into OpenGL context and save attributes
    LoadTexture(texIter->diffTexFile, &newTex.diffTex);
    LoadTexture(texIter->specTexFile, &newTex.specTex);
    newTex.gloss = texIter->gloss;

    // Store texture using name for key
    textures[texIter->texName] = newTex;
  }
}

// Creates models from model definitions
void ModelManager::CreateModels(std::vector<ModelDef> modDefs) {
  // Iterator for model definitions vector
  std::vector<ModelDef>::iterator modIter = modDefs.begin();
  // Iterate through vector until end
  for (; modIter != modDefs.end(); ++modIter) {
    // Calculating normal matrix
    glm::mat3 model3 = glm::mat3(modIter->modelMat);
    glm::mat3 norm = glm::transpose(glm::inverse(model3));
    // Creating new model and storing it, using name as key.
    Model newModel = Model{ modIter->meshName,
                            modIter->matName,
                            modIter->modelMat,
                            norm };
    models[modIter->modelName] = newModel;
  }
}

// Draws all models held in Model Manager's model map when called
void ModelManager::DrawModels(GLFWwindow* window) {
  // Creating iterator for models
  std::map<std::string, Model>::iterator modelIter = models.begin();
  // Increment iterator, getting the model name and drawing it each time
  for (; modelIter != models.end(); ++modelIter) {
    std::string modelName = modelIter->first;
    DrawModel(modelName, window);
  }
}
