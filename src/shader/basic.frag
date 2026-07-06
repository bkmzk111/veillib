
#version 460 core 

in vec2 o_TexCoords;

layout (location = 0) uniform vec3 u_Color;

layout (binding = 0) uniform sampler2D u_textureDiffuse1;
layout (binding = 1) uniform sampler2D u_textureSpecular1;

out vec4 o_FragColor;

void main() {
    
    o_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}