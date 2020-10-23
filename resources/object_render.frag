#version 330 core

struct light {
    vec3 position;
    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;
};

struct material {
    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;
    float shininess;
};

out vec4 FragmentColor;
in vec3 FragmentNormal;
in vec3 FragmentPosition;
in vec3 LightPosition; // struct light has position in it, but it needs to be calculated in Vertex shader.

uniform material FragLight;
uniform material FragMaterial;

vec3 AmbientColor() {
    return FragLight.ambient_color * FragMaterial.ambient_color;
}

vec3 LambertDiffuse(vec3 Normal) {
    vec3 LightDirection = normalize(LightPosition - FragmentPosition);
    float Intensity = max(dot(Normal, LightDirection), 0.0);

    return FragLight.diffuse_color * (Intensity * FragMaterial.diffuse_color);
}

vec3 PhongReflection(vec3 Normal) {
    vec3 LightDirection = normalize(LightPosition - FragmentPosition);
    vec3 ViewDirection = normalize(-FragmentPosition);

    float NdotL = dot(LightDirection, Normal);
    // vec3 Reflection = normalize(-LightDirection + 2.0 * NdotL * Normal);
    vec3 Reflection = reflect(-LightDirection, Normal);
    float Specular = pow(max(0.0, dot(ViewDirection, Reflection)), FragMaterial.shininess);

    return FragLight.specular_color * (Specular * FragMaterial.specular_color);
}

void main() {
    vec3 Normal = normalize(FragmentNormal);

    vec3 Ambient = AmbientColor();
    vec3 Diffuse = LambertDiffuse(Normal);
    vec3 Specular = PhongReflection(Normal);

    FragmentColor = vec4(Ambient + Diffuse + Specular, 1.0);
}
