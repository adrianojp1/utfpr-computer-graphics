#version 330 core

out vec4 fragment_colour;

in vec3 vert_pos_mesh;
in vec3 vert_pos_transformed;
 
uniform vec3 camera_position;
uniform vec2 scene_near_far;
uniform vec2 projection_near_far;

void main()
{
	vec3 colour = vec3(1.0f, 1.0f, 0.0f);

	float scene_near = scene_near_far.x;
	float scene_far = scene_near_far.y;
	float scene_light = 0.8 / (scene_near - scene_far) * (vert_pos_mesh.z - scene_far) + 0.1;

	float max_light_z = camera_position.z;
	float min_light_z = camera_position.z - projection_near_far.y;
	float camera_light = 1.0 / (max_light_z - min_light_z) * (vert_pos_transformed.z - min_light_z);
	camera_light = max(camera_light, 0.2);

	float light = camera_light * scene_light;

	fragment_colour = vec4(light * colour, 1.0);
}