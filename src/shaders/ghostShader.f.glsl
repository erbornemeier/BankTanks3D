#version 330 core

#version 330 core

in vec3 normalVec;
in vec3 lightVec;
in vec3 halfwayVec;
in vec2 texCoord;

const vec4 lightDiffuse = vec4(1.0, 1.0, 1.0, 1.0);
const vec4 lightSpecular = vec4(1.0, 1.0, 1.0, 1.0);
const vec4 lightAmbient = vec4(1.0, 1.0, 1.0, 1.0);

uniform vec4 materialDiffuse;
uniform vec4 materialSpecular;
uniform float materialShininess;
uniform vec4 materialAmbient;
uniform sampler2D txtr;

out vec4 fragColorOut;

void main() {
    
    fragColorOut = vec4(0.5,0.5,0.5,0.5);
}
