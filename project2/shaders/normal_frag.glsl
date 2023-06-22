#version 330 core

out vec4 fragColor;

in VS_OUT {
    vec3 frag_pos;
	vec3 tan_light_pos;
	vec3 tan_camera_pos;
	vec3 tan_frag_pos;
} fs_in;

uniform samplerCube diffuse_map;
uniform samplerCube normal_map;

uniform vec3 light_color;
uniform vec3 object_center;

void main()
{
    vec3 text_coord = fs_in.frag_pos - object_center;
    vec3 normal = textureCube(normal_map, text_coord).rgb * 2.0 - 1.0;
    normal.y = -normal.y;
    vec3 n = normalize(normal);
    
    float ka = 0.1;
    vec3 ambient = ka * light_color;

    float kd = 0.5;
    vec3 l = normalize(fs_in.tan_light_pos - fs_in.tan_frag_pos);

    float diff = max(dot(n,l), 0.0);
    vec3 diffuse = kd * diff * light_color;

    float ks = 0.8;
    vec3 v = normalize(fs_in.tan_camera_pos - fs_in.tan_frag_pos);
    vec3 r = reflect(-l, n);

    float spec = pow(max(dot(v, r), 0.0), 32);
    vec3 specular = ks * spec * light_color;

    vec3 color = textureCube(diffuse_map, text_coord).rgb;
    vec3 light = (ambient + diffuse + specular) * color;
    fragColor = vec4(light, 1.0);
}