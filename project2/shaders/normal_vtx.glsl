#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
 
out vec3 normal;
out vec3 frag_pos;
out vec3 transf_frag_pos;
 
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
 
void main()
{
	normal = mat3(transpose(inverse(model))) * aNormal;
	frag_pos = aPos;
	transf_frag_pos = vec3(model * vec4(aPos, 1.0));
	
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}