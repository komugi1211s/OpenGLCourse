#version 330 core
out vec4 FragmentColor;

in vec3 FragmentNormal;
in vec3 FragmentPosition;
in vec3 LightPosition;

uniform vec3 ObjectColor;
uniform vec3 LightColor;

vec3 AmbientColor() {
    float AmbientStrength = 0.1;
    return LightColor * AmbientStrength;
}

vec3 HalfLambertDiffuse(vec3 Normal) {
    float DiffuseStrength = 1.0;
    vec3 LightDirection = normalize(LightPosition - FragmentPosition);
    float Intensity = max(dot(Normal, LightDirection), 0.0);

    return Intensity * LightColor * DiffuseStrength;
}

vec3 PhongReflection(vec3 Normal) {
    float SpecularStrength = 8;
    float SpecularIntensity = 0.5;

    vec3 LightDirection = normalize(LightPosition - FragmentPosition);
    vec3 ViewDirection = normalize(-FragmentPosition);

    float NdotL = dot(LightDirection, Normal);
    vec3 Reflection = normalize(-LightDirection + 2.0 * NdotL * Normal);
    // vec3 Reflection = reflect(-LightDirection, Normal);
    float Specular = pow(max(0.0, dot(ViewDirection, Reflection)), SpecularStrength);

    return SpecularIntensity * Specular * LightColor;
}

void main() {
    vec3 Normal = normalize(FragmentNormal);

    vec3 Ambient = AmbientColor();
    vec3 Diffuse = HalfLambertDiffuse(Normal);
    vec3 Specular = PhongReflection(Normal);

    FragmentColor = vec4((Ambient + Diffuse + Specular) * ObjectColor, 1.0);
}
