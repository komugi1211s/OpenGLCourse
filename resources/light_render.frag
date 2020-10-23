#version 330 core
out vec4 FragmentColor;

struct light {
    vec3 position;
    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;
};

uniform light FragLight;

void main() {
    vec3 Normalized = normalize(FragLight.ambient_color + FragLight.diffuse_color + FragLight.specular_color);
    FragmentColor = vec4(Normalized * 2, 1.0);
}
