#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
 
out vec3 v_normal;
out vec3 frag_position;
 
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
 
void main()
{
	v_normal = mat3(transpose(inverse(model))) * aNormal;
	frag_position = vec3(model * vec4(aPos, 1.0));

	gl_Position = projection * view * model * vec4(aPos, 1.0);
}