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
    vec3 lightVec2 = normalize(lightVec);
    vec3 normalVec2 = normalize(normalVec);
    vec3 halfwayVec2 = normalize(halfwayVec);
    
    float sDotN = max( dot(lightVec2, normalVec2), 0.0 );
    vec4 diffuse = lightDiffuse * materialDiffuse * sDotN;
    
    vec4 specular = vec4(0.0);
    if( sDotN > 0.0 )
        specular = lightSpecular * materialSpecular * pow( max( 0.0, dot( halfwayVec2, normalVec2 ) ), materialShininess );
    
    vec4 ambient = lightAmbient * materialAmbient;
    
    fragColorOut = diffuse + specular + ambient;
    
    vec4 texel = texture( txtr, texCoord );
    fragColorOut *= texel;
}
