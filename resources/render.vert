#version 330 core
layout (location = 0) in vec3 vec_position;
layout (location = 1) in vec3 vec_color;
layout (location = 2) in vec3 vec_normal;

out vec3 VertexColor;
out vec3 FragmentNormal;
out vec3 FragmentPosition;
out vec3 LightPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 light_position;

void main() {
    gl_Position      = projection * view * model * vec4(vec_position, 1.0);
    VertexColor      = vec_color;
    FragmentNormal   = mat3(transpose(inverse(view * model))) * vec_normal;
    FragmentPosition = vec3(view * model * vec4(vec_position, 1.0));
    LightPosition    = vec3(view * vec4(light_position, 1.0));
}
