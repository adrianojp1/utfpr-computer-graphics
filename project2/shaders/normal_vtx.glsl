#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aTangent;

out VS_OUT {
	vec3 frag_pos;
	vec3 tan_light_pos;
	vec3 tan_camera_pos;
	vec3 tan_frag_pos;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 light_position;
uniform vec3 camera_position;

void main() {
	vs_out.frag_pos = aPos;

	mat3 normal_mat = transpose(inverse(mat3(model)));
    vec3 T = normalize(normal_mat * aTangent);
    vec3 N = normalize(normal_mat * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
	mat3 TBN = transpose(mat3(T, B, N));

	vs_out.tan_light_pos = TBN * light_position;
	vs_out.tan_camera_pos = TBN * camera_position;
	vs_out.tan_frag_pos = TBN * vec3(model * vec4(aPos, 1.0));

	gl_Position = projection * view * model * vec4(aPos, 1.0);
}