
#version 460 core 

layout (location = 0) uniform vec3 u_Color;

out vec4 o_FragColor;

void main() {
    
    o_FragColor = vec4(u_Color, 1.0);
}