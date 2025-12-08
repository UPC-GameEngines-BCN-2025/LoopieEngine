[vertex]
#version 460 core
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

out vec2  v_TexCoord;

uniform mat4 lp_Transform;

void main()
{
	gl_Position = lp_Projection * lp_View* lp_Transform * vec4(a_Position, 1.0);
	v_TexCoord = a_TexCoord;
}

[fragment]
#version 460 core
in vec2 v_TexCoord;

out vec4 FragColor;

uniform sampler2D u_Albedo;

void main()
{
	vec4 texColor = texture(u_Albedo, v_TexCoord);
	if(texColor.a < 0.5f)
		discard;
	FragColor = texColor;
}
