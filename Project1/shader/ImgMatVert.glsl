// Alice Norris, SNHU, Project 1 of CS-330 23EW3
// Excessive comments are meant to demonstrate understanding and not to explain
// the obvious. 

// This is the vertex shader for the shader program used on image-based
// textures, such as the dice or microphone body.
#version 330 core

//// IN: READ FROM VBO
// position
layout(location = 0) in vec3 inPos;
// normal
layout(location = 1) in vec3 inNorm;
// texture coordinate
layout(location = 2) in vec2 inTex;

//// UNIFORMS LOADED BY LOAD CALLS
// model matrix
uniform mat4 modelMat;
// normal matrix
uniform mat3 normMat; 

//// UNIFORMS LOADED BY BUFFER
layout(std140) uniform camData{
  // view matrix of camera
  uniform mat4 view;
  // proj matrix of camera
  uniform mat4 proj;
  // camera position ( cast to vec 3)
  uniform vec4 camPos;
};

//// OUT: LEAVING TO FRAGMENT SHADER
// fragment position
out vec3 fragPosVec;
// normal 
out vec3 normVec;
// texture coordinate
out vec2 texCoord;
// camera position
out vec3 viewPos;

void main()
{
  // casting frag position to vec3 after calculating from model matrix
  fragPosVec = vec3(modelMat * vec4(inPos, 1.0f));
  // calculating normal vector 
  normVec = normalize(normMat * inNorm);
  // sending on tex coordinate as is
  texCoord = inTex;
  // sending on camera position as vec3
  viewPos = vec3(camPos);
  // calculating gl position
  gl_Position = proj * view * vec4(fragPosVec, 1.0);
}