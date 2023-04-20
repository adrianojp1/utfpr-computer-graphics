#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
 
out vec3 vert_pos_mesh;
out vec3 vert_pos_transformed;
 
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
 
void main()
{
	vert_pos_mesh = aPos;
	vert_pos_transformed = vec3(model * vec4(aPos, 1.0));
	
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}