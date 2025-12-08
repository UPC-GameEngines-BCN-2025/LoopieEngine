[vertex]
#version 460 core
/// DO NOT MODIFY
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoord;
layout (location = 2) in vec3 a_Normal;
layout (location = 3) in vec3 a_Tangent;
layout (location = 4) in vec4 a_Color;

layout (std140, binding = 0) uniform Matrices
{
    mat4 lp_Projection;
    mat4 lp_View;
};

uniform mat4 lp_Transform;
///

void main()
{
    gl_Position = lp_Projection * lp_View * lp_Transform * vec4(a_Position, 1.0);
}


[fragment]
#version 460 core
out vec4 FragColor;

uniform vec4 u_Color = vec4(0.0, 0.5, 0.75, 1.0);

void main()
{
    FragColor = u_Color;
} 
