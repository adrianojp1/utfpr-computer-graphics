#version 330 core

out vec4 fragment_colour;

in vec3 vert_pos_mesh;
in vec3 vert_pos_transformed;
 
uniform vec3 camera_position;
uniform vec2 scene_near_far;
uniform vec2 projection_near_far;

void main()
{
	float scene_near = scene_near_far.x;
	float scene_far = scene_near_far.y;
	float colour = 0.8 / (scene_near - scene_far) * (vert_pos_mesh.z - scene_far) + 0.1;

	float max_light_z = camera_position.z;
	float min_light_z = camera_position.z - (projection_near_far.y / 10);
	float linear_light = 1.0 / (max_light_z - min_light_z) * (vert_pos_transformed.z - min_light_z);
	float light = max(linear_light, 0.2);

	fragment_colour = vec4(light * vec3(colour), 1.0);
}