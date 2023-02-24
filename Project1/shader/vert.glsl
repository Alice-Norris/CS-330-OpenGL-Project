#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec2 aTex;

#define MAX_MODELS 32

struct Model {
  // gloss
  float gloss;

  // Matrices
  mat4 modelMat;
  mat3 normMat;
};

uniform int modelIndex;
uniform mat4 view;
uniform mat4 proj;
uniform Model models[MAX_MODELS];  // Max 32

out float matGloss;
out vec2 texCoord;
out vec3 fragPos;
out vec3 normVec;


void main()
{
  Model curr_model = models[modelIndex];
  fragPos = vec3(curr_model.modelMat * vec4(aPos, 1.0f));
  normVec = normalize(curr_model.normMat * aNorm);
  texCoord = aTex;
  matGloss = curr_model.gloss;
  gl_Position = proj * view * curr_model.modelMat * vec4(aPos, 1.0);
}