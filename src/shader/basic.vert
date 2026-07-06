
#version 460 core 

layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoords;

layout (location = 1) uniform mat4 u_Projection;
layout (location = 5) uniform mat4 u_View;
layout (location = 9) uniform mat4 u_Model;

out vec2 o_TexCoords;

void main() {

    gl_Position = u_Projection * u_View * u_Model * vec4(a_Pos, 1.0);

    o_TexCoords = a_TexCoords;
}