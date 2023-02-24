// Alice Norris, SNHU, Project 1 of CS-330 23EW3
// Excessive comments are meant to demonstrate understanding and not to explain
// the obvious. 

// This is the fragment shader for the shader program used on property-based
// materials, such as plain metal, unpainted plastic, and the like.
#version 330 core
#define MAX_P_LIGHTS 8
#define MAX_D_LIGHTS 8
// Defines material attributes
struct Material {
  // Material colors
  vec3 amb;
  vec3 diff;
  vec3 spec;
  // Material shininess factor
  float gloss;
};

// Defines directional light attributes
struct DirLight {
  // Light direction
  vec4 dir;
  // Light colors
  vec4 amb;
  vec4 diff;
  vec4 spec;
  // Light intensity factor
  float intensity;
};

// Defines point light attributes
struct PntLight {
  // Light position
  vec4 pos;
  // Light colors
  vec4 amb;
  vec4 diff;
  vec4 spec;
  // Attenuation attributes
  float constVal;
  float linVal;
  float quadVal;
  // Light intensity factor
  float intensity;
};

//// IN: COMING FROM VERTEX SHADER
// fragment position vector
in vec3 fragPosVec;
// normal vector
in vec3 normVec;
// position of camera
in vec3 viewPos;

////// UNIFORM: LOADED BY LOAD CALL
// Material for the model
uniform Material material;

//// UNIFORM: LOADED BY BUFFER
// Dirlight data and number of dir lights
layout (std140) uniform DirLights {
  DirLight dirLights[MAX_D_LIGHTS];
  int numDirLights;
};

// PntLight data and number of pnt lights
layout (std140) uniform PntLights{
  PntLight pntLights[MAX_P_LIGHTS];
  int numPntLights;
};

//// OUT: LEAVING VERTEX SHADER
out vec4 FragColor;

// Function for directional light calculations
vec3 dirLightCalc(DirLight light, vec3 viewDir);

// Function for point light calculations
vec3 pntLightCalc(PntLight light, vec3 viewDir);

void main() {
  // Calculating view direction
  vec3 viewDir = normalize(viewPos - fragPosVec);
  
  // initializing result
  vec3 result = vec3(0.0f, 0.0f, 0.0f);
  
  // calculate lighting results for each directional light, adding cumulatively
  for (int i = 0; i < numDirLights; ++i) {
    DirLight curr_light = dirLights[i];
    result += dirLightCalc(curr_light, viewDir);
  }
  // calculate lightingg results for each point light, adding cumulatively
  for (int i = 0; i < numPntLights; ++i) {
    PntLight curr_light = pntLights[i];
    result += pntLightCalc(curr_light, viewDir);
  }
  // output
  FragColor = vec4(result, 1.0);
}

// Calculates fragment color for a given dir light
vec3 dirLightCalc(DirLight light, vec3 viewDir) {
  // calculating light and reflect direction vectors
  vec3 lightDir = normalize(vec3(-light.dir));
  vec3 reflectDir = reflect(-lightDir, normVec);

  // calculating diffuse factor
  float diffVal = max(dot(normVec, lightDir), 0.0);

  // calculating specular factor
  float specVal = pow(max(dot(viewDir, reflectDir), 0.0), material.gloss);

  // calculating ambient, diffuse, sepcular colors
  vec3 ambient = vec3(light.amb) * material.amb;
  vec3 diffuse = vec3(light.diff) * diffVal * material.diff;
  vec3 specular = vec3(light.spec) * specVal * material.spec;

  return ((ambient * light.intensity)
         + (diffuse * light.intensity)
         + (specular * light.intensity));
}

// Calculates fragment color for a given point light
vec3 pntLightCalc(PntLight light, vec3 viewDir) {
  // calculating light and reflection direction vectors
  vec3 lightDir = normalize(vec3(light.pos) - fragPosVec);
  vec3 reflectDir = reflect(-lightDir, normVec);

  // calculating light distance and attenuation
  float distance = length(vec3(light.pos) - fragPosVec);
  float attenuation = 1.0 / (light.constVal
                             + light.linVal * distance
                             + (light.quadVal * (pow(distance, 2))));

  // Calculating diff and spec factors
  float diff = max(dot(normVec, lightDir), 0.0);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.gloss);

  // Calculating ambient, diffuse, and sepcular colors
  vec3 ambient = vec3(light.amb) * material.amb;
  vec3 diffuse = vec3(light.diff) * (diff * material.diff);
  vec3 specular = vec3(light.spec) * (spec * material.spec);

  // Applying attenuation
  ambient *= attenuation;
  diffuse *= attenuation;
  specular *= attenuation;
  return ((ambient * light.intensity) 
          + (diffuse * light.intensity)
          + (specular * light.intensity));
}