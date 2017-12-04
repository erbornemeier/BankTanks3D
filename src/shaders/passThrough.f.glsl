#version 330 core

in vec2 texCoord;

uniform sampler2D fbo;

out vec4 fragColorOut;

void main() {
    fragColorOut = texture( fbo, texCoord );
    fragColorOut *= vec4(0.8,0.9,1.0,1.0);
    fragColorOut += vec4(0.1,0.1,0.1,1.0);
    fragColorOut.z = clamp(fragColorOut.z, 0.0, 1.0);
}
