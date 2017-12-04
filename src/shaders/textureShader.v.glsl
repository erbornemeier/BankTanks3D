#version 330 core

in vec3 vPos;
in vec2 vTextureCoord;

out vec2 texCoord;

uniform mat4 modelMtx;
uniform mat4 viewProjectionMtx;

void main() {
  gl_Position = viewProjectionMtx * modelMtx * vec4(vPos, 1.0);
  texCoord = vTextureCoord;
}
