#version 330 core

in vec3 v_normal;
in vec3 frag_position;

uniform vec3 object_color;
uniform vec3 light_color;
uniform vec3 light_position;
uniform vec3 camera_position;

out vec4 frag_color;

void main()
{
	float ka = 0.5;
    vec3 ambient = ka * light_color;

    float kd = 0.7;
    vec3 n = normalize(v_normal);
    vec3 l = normalize(light_position - frag_position);

    float diff = max(dot(n,l), 0.0);
    vec3 diffuse = kd * diff * light_color;

    float ks = 1.0;
    vec3 v = normalize(camera_position - frag_position);
    vec3 r = reflect(-l, n);

    float spec = pow(max(dot(v, r), 0.0), 32);
    vec3 specular = ks * spec * light_color;

    vec3 light = (ambient + diffuse + specular) * object_color;
    frag_color = vec4(light, 1.0);
}