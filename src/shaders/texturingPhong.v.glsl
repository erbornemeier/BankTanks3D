#version 330 core

in vec3 vPos;
in vec3 vNormal;
in vec2 vTexCoord;

out vec3 normalVec;
out vec3 lightVec;
out vec3 halfwayVec;
out vec2 texCoord;

uniform mat4 modelviewMtx;
uniform mat4 viewMtx;	
uniform mat4 projectionMtx;
uniform mat4 normalMtx;

const vec3 lightPos = vec3( 0.0, 10.0, 0.0 );

void main() {
    gl_Position = projectionMtx * modelviewMtx * vec4(vPos, 1.0);
    
    vec3 cameraVec = normalize( -(modelviewMtx*vec4(vPos,1.0)).xyz );
    normalVec = normalize( (normalMtx * vec4(vNormal,0.0)).xyz );
    lightVec = normalize( (viewMtx * vec4(lightPos,1.0)).xyz - (modelviewMtx*vec4(vPos,1.0)).xyz );
    halfwayVec = normalize( cameraVec + lightVec );
    
    texCoord = vTexCoord;
}
