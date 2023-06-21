#version 330 core

in vec3 frag_pos;
in vec3 normal;
in vec3 transf_frag_pos;

out vec4 frag_color;

uniform vec3 light_color;
uniform vec3 light_position;
uniform vec3 camera_position;
uniform vec3 object_center;
uniform samplerCube diffuse_map;

void main()
{
    float ka = 0.1;
    vec3 ambient = ka * light_color;

    float kd = 0.5;
    vec3 n = normalize(normal);
    vec3 l = normalize(light_position - transf_frag_pos);

    float diff = max(dot(n,l), 0.0);
    vec3 diffuse = kd * diff * light_color;

    float ks = 0.8;
    vec3 v = normalize(camera_position - transf_frag_pos);
    vec3 r = reflect(-l, n);

    float spec = pow(max(dot(v, r), 0.0), 32);
    vec3 specular = ks * spec * light_color;

    vec3 object_color = textureCube(diffuse_map, frag_pos - object_center).rgb;
    vec3 light = (ambient + diffuse + specular) * object_color;
    frag_color = vec4(light, 1.0);
}