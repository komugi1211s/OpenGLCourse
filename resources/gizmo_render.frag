#version 330 core

in  vec3 VertexColor;
out vec4 FragmentColor;

void main() {
    FragmentColor = vec4(VertexColor, 1.0);
}
