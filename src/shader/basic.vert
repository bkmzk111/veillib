
#version 460 core 

layout (location = 0) in vec3 a_Pos;

layout (location = 1) uniform mat4 u_Projection;
layout (location = 2) uniform mat4 u_View;
layout (location = 3) uniform mat4 u_Model;

void main() {

    gl_Position = u_Projection * u_View * u_Model * vec4(a_Pos, 1.0);
}