#version 330 core

in vec3 frag_position;

out vec4 frag_color;

uniform samplerCube cube;
uniform vec3 object_center;

void main()
{
    // vec3 text_dir = vec3(vec2(frag_position), 1.0);
    frag_color = textureCube(cube, frag_position - object_center);
}