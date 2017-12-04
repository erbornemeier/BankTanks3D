#version 330 core

in vec3 vPos;
in vec2 vTexCoord;

out vec2 texCoord;

uniform mat4 projectionMtx;

void main() {
    gl_Position = projectionMtx *  vec4(vPos, 1.0);
    
    texCoord = vTexCoord;
}