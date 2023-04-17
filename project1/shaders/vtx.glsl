#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
 
out vec3 vert_pos_varying;
out vec3 vert_pos_transformed;
out vec3 vertex_normal;
 
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
 
void main()
{
	vert_pos_varying = aPos;
	vert_pos_transformed = vec3(model * vec4(aPos, 1.0));
 
	mat3 normal_matrix = transpose(inverse(mat3(model)));
	vertex_normal = normal_matrix * aNormal;
	
	if (length(vertex_normal) > 0) {
		vertex_normal = normalize(vertex_normal);
	}
		
	// https://www.khronos.org/opengl/wiki/Vertex_Post-Processing
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}