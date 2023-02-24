// Alice Norris, SNHU, Project 1 of CS-330 23EW3
// All code follows Google's style guide as closely as possible
// without sacrificing clarity. Excessive comments are meant to
// demonstrate understanding and not to explain the obvious.

#include "Camera.h"
#include "Lights.h"
#include "ModelManager.h"
#include "Shader.h"
#include "WindowManager.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

// Prints shader info log when debugging
void PrintProgramInfoLog(Shader* shader);

// Called every frame, renders models
void Render(GLFWwindow* window);

namespace {
  // microphone position
  // (applied to all models involved to move the entire object)
  glm::vec3 micPos = glm::vec3(24.4843, 0.0, -19.2146);

  // microphone base rotation, used to position the legs
  float micBaseRot = 58.0f;

  // microphone top rotation, used to position mic's upper half to
  // correct yaw
  float micTopRot = 22.5f;

  // Window width/height
  GLfloat kWinHeight = 800.0f;
  GLfloat kWinWidth = 600.0f;

  // Handles input, callbacks, window creation, and GLFW/GLEW initialization.
  WindowManager winMgr = WindowManager(kWinHeight, kWinWidth,
                                       "Alice Norris Project 1", false);

  // Getting our window pointer as soon as possible
  GLFWwindow* window = winMgr.GetWinPtr();

  // Creates, loads, and stores materials, textures, and meshes.
  // Creates, draws, and stores models
  ModelManager modMgr = ModelManager();

  // Shader programs. One is used to handle image materials (such as dice),
  // and the other is used to handle property based materials 
  // (such as plain metal).
  Shader imgMatShader("ImgMatVert.glsl", "ImgMatFrag.glsl");
  Shader propMatShader("PropMatVert.glsl", "PropMatFrag.glsl");

  // Scene's camera.
  Camera sceneCam = Camera(glm::vec3(0.0f, 0.5f, -3.0f),
                           glm::vec3(0.0f, 0.0f, -2.0f),
                           kWinHeight, kWinWidth);

  // Array of pointers to objects, set as window pointer
  // so objects can talk to each other
  const void* objPtrs[5] = {
    &winMgr,
    &modMgr,
    &imgMatShader,
    &propMatShader,
    &sceneCam
  };

  // Vector of filenames for each mesh's file
  std::vector<std::string> meshFiles{
    "d6.dae", "d8.dae", "d20.dae",
    "desk.dae", "mic_base.dae", "mic_body.dae", "mic_feet.dae",
    "mic_filt_cmp.dae", "mic_gain_knob.dae",
    "mic_hold.dae", "mic_leg.dae", "mic_swivel.dae", "wall.dae"
  };

  // Definitions for each material used in scene.
  // Format: material name, ambient, diffuse, specular, gloss values.
  std::vector<ModelManager::MaterialDef> materials = {
    {
      "blk_rubber",
      glm::vec3(0.02f, 0.02f, 0.02f),
      glm::vec3(0.01f, 0.01f, 0.01f),
      glm::vec3(0.4f, 0.4f, 0.4f),
      0.25f},
    {
      "blk_plastic",
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.01f, 0.01f, 0.01f),
      glm::vec3(0.5f, 0.5f, 0.5f),
      0.78f
    },
    {
      "slv_chrome",
      glm::vec3(0.33f, 0.33f, 0.33f),
      glm::vec3(0.4f, 0.4f, 0.4f),
      glm::vec3(0.85f, 0.85f, 0.85f),
      0.16f
    }
  };

  // Definitions for each texture used in scene.
  // Format: texture name, diffuse filename, specular filename, material gloss
  std::vector<ModelManager::TextureDef> textures = {
    {"d6_tex", "d6_diff.png", "d6_spec.png", 0.86f},
    {"d8_tophalf_tex", "d8_tophalf_diff.png", "d8_tophalf_spec.png", 0.86f},
    {"d8_bothalf_tex", "d8_bothalf_diff.png", "d8_bothalf_spec.png", 0.86f},
    {"d20_tex", "d20_diff.png", "d20_spec.png", 0.86f},
    {"mic_filt_tex", "mic_filt_diff.png", "mic_filt_spec.png", 0.64f},
    {"mic_gain_tex", "mic_gain_diff.png", "metal_spec.png", 0.16f},
    {"mic_body_tex", "mic_body_diff.png", "metal_spec.png", 0.16f},
    {"wall_tex", "drywall_diff.png", "drywall_spec.png", 0.64f},
    {"desk_tex", "desk_diff.png", "desk_spec.png", 0.64f}
  };

  // Definitions for each model used in scene.
  // Format: model name, mesh name, material/texture name, model matrix
  std::vector<ModelManager::ModelDef> models = {
    {"desk", "desk", "desk_tex", glm::mat4(1.0f)},
    {"wall", "wall", "wall_tex", glm::mat4(1.0f)},
    {"micFoot1", "mic_feet", "blk_rubber", glm::mat4(1.0f)},
    {"micFoot2", "mic_feet", "blk_rubber", glm::mat4(1.0f)},
    {"micFoot3", "mic_feet", "blk_rubber", glm::mat4(1.0f)},
    {"micLeg1", "mic_leg", "slv_chrome", glm::mat4(1.0f)},
    {"micLeg2", "mic_leg", "slv_chrome", glm::mat4(1.0f)},
    {"micLeg3", "mic_leg", "slv_chrome", glm::mat4(1.0f)},
    {"mic_base", "mic_base", "blk_plastic", glm::mat4(1.0f)},
    {"mic_swivel", "mic_swivel", "blk_plastic", glm::mat4(1.0f)},
    {"mic_hold", "mic_hold", "blk_plastic", glm::mat4(1.0f)},
    {"mic_body", "mic_body", "mic_body_tex", glm::mat4(1.0f)},
    {"mic_filt_cmp", "mic_filt_cmp", "mic_filt_tex", glm::mat4(1.0f)},
    {"mic_gain_knob", "mic_gain_knob", "mic_gain_tex", glm::mat4(1.0f)},
    {"d6", "d6", "d6_tex", glm::mat4(1.0f)},
    {"d8_tophalf", "d8", "d8_tophalf_tex", glm::mat4(1.0f)},
    {"d8_bothalf", "d8", "d8_bothalf_tex", glm::mat4(1.0f)},
    {"d20", "d20", "d20_tex", glm::mat4(1.0f)}
  };

};  // namespace

int main() {
  // Putting pointer array in window so other objects can access o
  // Other objects' data.
  glfwSetWindowUserPointer(winMgr.GetWinPtr(), &objPtrs);

  // Calculating matrices for each model, applying appropriate
  // transformations

  // mic foot 1 matrix
  models.at(2).modelMat =
    glm::translate(micPos)
    * glm::rotate(glm::radians(micBaseRot), glm::vec3(0.0f, 1.0f, 0.0f))
    * glm::translate(glm::vec3(7.5326, 0.1429f, -0.0000f))
    * glm::rotate(glm::radians(50.0f), glm::vec3(0.0f, 0.0f, 1.0f));

  // mic foot 2 matrix
  models.at(3).modelMat =
    glm::translate(micPos)
    * glm::rotate(glm::radians(micBaseRot), glm::vec3(0.0f, 1.0f, 0.0f))
    * glm::translate(glm::vec3(-3.7663f, 0.1429f, -6.5234f))
    * glm::rotate(glm::radians(120.0f), glm::vec3(0.0f, 1.0f, 0.0f))
    * glm::rotate(glm::radians(50.0f), glm::vec3(0.0f, 0.0f, 1.0f));

  //// mic foot 3 matrix
  models.at(4).modelMat =
    glm::translate(micPos)
    * glm::rotate(glm::radians(micBaseRot), glm::vec3(0.0f, 1.0f, 0.0f))
    * glm::translate(glm::vec3(-3.7663f, 0.1429f, 6.5234f))
    * glm::rotate(glm::radians(240.0f), glm::vec3(0.0f, 1.0f, 0.0f))
    * glm::rotate(glm::radians(50.0f), glm::vec3(0.0f, 0.0f, 1.0f));

  //// mic leg 1 matrix
  models.at(5).modelMat =
    glm::translate(micPos)
    * glm::rotate(glm::radians(micBaseRot), glm::vec3(0.0f, 1.0f, 0.0f))
    * glm::translate(glm::vec3(6.5480f, 0.9690f, -0.0043f))
    * glm::rotate(glm::radians(50.0f), glm::vec3(0.0f, 0.0f, 1.0f));

  // mic leg 2 matrix
  models.at(6).modelMat =
    glm::translate(micPos)
    * glm::rotate(glm::radians(micBaseRot), glm::vec3(0.0f, 1.0f, 0.0f))
    * glm::translate(glm::vec3(-3.2924f, 0.9690f, -5.6941f))
    * glm::rotate(glm::radians(120.0f), glm::vec3(0.0f, 1.0f, 0.0f))
    * glm::rotate(glm::radians(50.0f), glm::vec3(0.0f, 0.0f, 1.0f));

  // mic leg 3 matrix
  models.at(7).modelMat =
    glm::translate(micPos)
    * glm::rotate(glm::radians(micBaseRot), glm::vec3(0.0f, 1.0f, 0.0f))
    * glm::translate(glm::vec3(-3.2905f, 0.9690f, 5.6964f))
    * glm::rotate(glm::radians(240.0f), glm::vec3(0.0f, 1.0f, 0.0f))
    * glm::rotate(glm::radians(50.0f), glm::vec3(0.0f, 0.0f, 1.0f));

  // mic base matrix
  models.at(8).modelMat =
    glm::translate(micPos + glm::vec3(0.0f, 4.95f, 0.0f));

  // mic swivel matrix
  models.at(9).modelMat =
    glm::translate(micPos)
    * glm::rotate(glm::radians(micTopRot), glm::vec3(0.0f, 1.0f, 0.0f))
    * glm::translate(glm::vec3(0.0000f, 6.1500f, 0.0000f));

  // mic holder matrix
  models.at(10).modelMat =
    glm::translate(micPos)
    * glm::rotate(glm::radians(micTopRot), glm::vec3(0.0f, 1.0f, 0.0f))
    * glm::translate(glm::vec3(0.0000f, 9.1533f, 0.000f))
    * glm::rotate(glm::radians(56.0f), glm::vec3(0.0f, 0.0f, 1.0f));

  // mic body matrix
  models.at(11).modelMat =
    glm::translate(micPos)
    * glm::rotate(glm::radians(micTopRot), glm::vec3(0.0f, 1.0f, 0.0f))
    * glm::translate(glm::vec3(2.7499f, 12.9520f, 0.0104f))
    * glm::rotate(glm::radians(56.0f), glm::vec3(0.0f, 0.0f, 1.0f))
    * glm::rotate(glm::radians(90.00f), glm::vec3(0.0f, 1.0f, 0.0f));

  // mic filter body
  models.at(12).modelMat =
    glm::translate(micPos)
    * glm::rotate(glm::radians(micTopRot), glm::vec3(0.0f, 1.0f, 0.0f))
    * glm::translate(glm::vec3(-1.394f, 15.752f, 0.0106f))
    * glm::rotate(glm::radians(56.0f), glm::vec3(0.0f, 0.0f, 1.0f))
    * glm::scale(glm::vec3(1.33f, 1.0f, 1.125f));

  // mic gain knob
  models.at(13).modelMat =
    glm::translate(micPos)
    * glm::rotate(glm::radians(micTopRot), glm::vec3(0.0f, 1.0f, 0.0f))
    * glm::translate(glm::vec3(2.1357f, 15.6f, 0.0106f))
    * glm::rotate(glm::radians(-34.0f), glm::vec3(0.0f, 0.0f, 1.0f));

  // d6 matrix
  models.at(14).modelMat =
    glm::translate(glm::vec3(16.936f, 0.5800f, -11.7060f))
    * glm::rotate(glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f))
    * glm::rotate(glm::radians(205.0f), glm::vec3(0.0f, 1.0f, 0.0f));

  // d8 tophalf matrix
  models.at(15).modelMat =
    glm::translate(glm::vec3(22.8730f, 0.5834f, -10.4779f))
    * glm::rotate(glm::radians(-125.0f), glm::vec3(0.0f, 0.0f, 1.0f))
    * glm::rotate(glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));

  // d8 bothalf matrix
  models.at(16).modelMat =
    glm::translate(glm::vec3(22.8730f, 0.5834f, -10.4779f))
    * glm::rotate(glm::radians(55.0f), glm::vec3(0.0f, 0.0f, 1.0f))
    * glm::rotate(glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));

  // d20 matrix
  models.at(17).modelMat =
      glm::translate(glm::vec3(21.5720f, 0.0f, -14.792f))
    * glm::rotate(glm::radians(-95.0f), glm::vec3(0.0f, 1.0f, 0.0f))
    * glm::translate(glm::vec3(0.0f, 0.8f, 0.0f))
    * glm::rotate(glm::radians(-30.5f), glm::vec3(0.0f, 0.0f, 1.0f))
    * glm::rotate(glm::radians(66.0f), glm::vec3(1.0f, 0.0f, 0.0f));

  // Creating materials, textures, meshes, and finally models
  // These are all stored, held, and used by the Model Manager class
  modMgr.CreateMaterials(materials);
  modMgr.CreateTextures(textures);
  modMgr.CreateMeshes(meshFiles);
  modMgr.CreateModels(models);

  // Binding and loading initial camera data.
  sceneCam.BindCamData(window);

  // Loading light data into uniform buffers
  LoadDirLights(window);
  LoadPntLights(window);

  // Setting background color of 3D space
  glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

  // Repeat render loop until we receive a close signal from GLFW
  while (!winMgr.closeCheck()) {
    Render(window);
  }
}

void Render(GLFWwindow* window) {
  // Update camera's frame time
  sceneCam.updateTime();

  // Clear depth and color buffer back to presets
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Loops through all the models created earlier, drawing each
  modMgr.DrawModels(window);

  // Process events in event queue (such as callbacks)
  glfwPollEvents();

  // Swap front and back buffers of the window
  glfwSwapBuffers(window);
}

// Prints program info log when debugging
void PrintProgramInfoLog(Shader* shader) {
  // Setting up variables for array size and actual log character array
  GLsizei logLen = 0;
  GLchar* infoLog = new GLchar[512];

  // Get the log
  glGetProgramInfoLog(shader->id, 512, &logLen, infoLog);

  // Don't bother if there's nothing to print
  if (logLen != 0) {
    std::cout << infoLog << std::endl;
  }
}
