#version 330 core

#define MAX_P_LIGHTS 8
#define MAX_D_LIGHTS 8


struct DirLight {
  vec3 dir;

  vec3 amb;
  vec3 diff;
  vec3 spec;

  float intensity;
};

struct PtLight {
  vec3 pos;

  vec3 amb;
  vec3 diff;
  vec3 spec;

  float constVal;
  float linVal;
  float quadVal;
  float intensity;
};

in float matGloss;
in vec2 texCoord;
in vec3 fragPos;
in vec3 normVec;

// Initializing array to constant maximums
uniform DirLight dirLights[MAX_D_LIGHTS];  // Max 8
uniform PtLight ptLights[MAX_P_LIGHTS];  // Max 8


// Actual number of lights and models for each array
uniform int numDirLights;
uniform int numPtLights;
uniform sampler2D diffSamp;
uniform sampler2D specSamp;

// Position of viewer
uniform vec3 viewPos;

out vec4 fragColor;

vec3 dirLightCalc(DirLight light, float gloss, vec3 normVec, vec3 viewDir);

vec3 ptLightCalc(PtLight light, float gloss, vec3 normVec, vec3 fragPos, vec3 viewDir);


void main() {
  // Calculating view direction
  vec3 viewDir = normalize(viewPos - fragPos);

  // initializing result
  vec3 result = vec3(0.0f, 0.0f, 0.0f);

  // calculate lighting results for each directional light, adding cumulatively
  for (int i = 0; i < numDirLights; ++i) {
    result += dirLightCalc(dirLights[i], matGloss, normVec, viewDir);
  }

  // calculate lightingg results for each point light, adding cumulatively
  for (int i = 0; i < numPtLights; ++i) {
    result += ptLightCalc(ptLights[i], matGloss, normVec, fragPos, viewDir);
  }

  // output
  fragColor = vec4(result, 1.0);
}

vec3 dirLightCalc(DirLight light, float gloss, vec3 normVec, vec3 viewDir) {
  // calculating light and reflect direction vectors
  vec3 lightDir = normalize(-light.dir);
  vec3 reflectDir = reflect(-lightDir, normVec);

  // calculating diff factor
  float diff = max(dot(normVec, lightDir), 0.0);

  // calculating specular factor
  float viewReflectProduct = dot(viewDir, reflectDir);
  viewReflectProduct = max(viewReflectProduct, 0.0);
  float spec = pow(viewReflectProduct, gloss);

  // calculatingg ambient, diffuse, specular colors
  vec3 ambient = light.amb * vec3(texture(diffSamp, texCoord));
  vec3 diffuse = light.diff * diff * vec3(texture(diffSamp, texCoord));
  vec3 specular = light.spec * spec * vec3(texture(specSamp, texCoord));

  // scaling light value by intensity
  diffuse *= light.intensity;
  specular *= light.intensity;

  // returning result
  return (ambient + diffuse + specular);
}

vec3 ptLightCalc(PtLight light, float gloss, vec3 normVec, vec3 fragPos, vec3 viewDir) {
  // calculating light and reflection direction vectors
  vec3 lightDir = normalize(-light.pos - fragPos);
  vec3 reflectDir = reflect(-lightDir, normVec);

  // calculating light distance and attenuation
  float distance = length(light.pos - fragPos);
  float attenuation = 1.0 / (light.constVal + light.linVal * distance + light.quadVal * pow(distance, 2));

  // calculating diff and spec factors
  float diff = max(dot(normVec, lightDir), 0.0);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), gloss);

  // calculating ambient, diffuse, and sepcular colors
  vec3 ambient = light.amb * vec3(texture(diffSamp, texCoord));
  vec3 diffuse = light.diff * diff * vec3(texture(diffSamp, texCoord));
  vec3 specular = light.spec * spec * vec3(texture(specSamp, texCoord));

  // applying intensity
  diffuse *= light.intensity;
  specular *= light.intensity;

  // applying attenuation
  ambient *= attenuation;
  diffuse *= attenuation;
  specular *= attenuation;

  // output result
  return (ambient + diffuse + specular);
}