#version 330 core

in vec2 texCoord;

out vec4 fragColorOut;

uniform sampler2D tex;
uniform vec3 color;

void main() {
  fragColorOut = texture( tex, texCoord ) * vec4(color, 1);
}
