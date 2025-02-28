#include "mafs.h"
#include "render.h"
#ifndef MY_ALLOCA_H_
#define MY_ALLOCA_H_
#include <alloca.h> // for alloca
#endif

internal Texture_Info
load_image_file_into_texture(char *file_name, GLenum texture_type) {
    Texture_Info info = {0};
    info.texture_type = texture_type;

    u8 *file_data = stbi_load(file_name, &info.width, &info.height, &info.channels, 0);
    if (!file_data) {
        Texture_Info empty = {0};
        return(empty);
    }

    glGenTextures(1, &info.id);
    glBindTexture(texture_type, info.id);

    // TODO: GL_TEXTURE_WRAP_S, _T, _R are hard-coded
    glTexParameteri(texture_type, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(texture_type, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(texture_type, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);

    glTexImage2D(texture_type, 0, GL_RGB, info.width, info.height, 0, GL_RGB, GL_UNSIGNED_BYTE, file_data);
    glGenerateMipmap(texture_type);

    stbi_image_free(file_data);
    return(info);
}

internal void
use_texture(Texture_Info *tex, GLenum texture_unit) {
    glActiveTexture(texture_unit);
    glBindTexture(tex->texture_type, tex->id);
}

internal Shader_Info
initialize_shaders(const char *vtx_shader_src, const char *frag_shader_src) {
    Shader_Info program = {0};
    u32 vert_shader, frag_shader;
    i32 success = 0;
    char error_log[512] = {0};

    vert_shader = glCreateShader(GL_VERTEX_SHADER);
    frag_shader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vert_shader, 1, &vtx_shader_src, 0);
    glCompileShader(vert_shader);

    glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vert_shader, 512, 0, error_log);
        printf("VertShader: %s\n", error_log);

        glDeleteShader(vert_shader);
        glDeleteShader(frag_shader);
        return(program);
    }
    success = 0;

    glShaderSource(frag_shader, 1, &frag_shader_src, 0);
    glCompileShader(frag_shader);

    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(frag_shader, 512, 0, error_log);
        printf("FragShader: %s\n", error_log);
        glDeleteShader(vert_shader);
        glDeleteShader(frag_shader);
        return(program);
    }
    success = 0;

    program.id  = glCreateProgram();
    glAttachShader(program.id, vert_shader);
    glAttachShader(program.id, frag_shader);
    glLinkProgram(program.id);

    glGetProgramiv(program.id, GL_LINK_STATUS, &success);
    if (!success) {
        glDeleteShader(vert_shader);
        glDeleteShader(frag_shader);

        glGetProgramInfoLog(program.id, 512, 0, error_log);
        printf("Program: %s\n", error_log);

        memset(&program, 0, sizeof(program));
        return(program);
    }

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    glUseProgram(program.id);
    return(program);
}

internal void
use_shader(Shader_Info *shader) {
    glUseProgram(shader->id);
}

internal void
set_uniform_matrix4x4(Shader_Info *shader, char *uniform_var_name, mat4x4 *passing_matrix) {
    GLuint target;

    target = glGetUniformLocation(shader->id, uniform_var_name);
    glUniformMatrix4fv(target, 1, GL_FALSE, (f32 *)passing_matrix);
}

internal void
set_uniform_vec3(Shader_Info *shader, char *uniform_var_name, v3 *passing_vec3) {
    GLuint target;
    target = glGetUniformLocation(shader->id, uniform_var_name);
    glUniform3fv(target, 1, (f32 *)passing_vec3);
}

internal void
set_uniform_vec4(Shader_Info *shader, char *uniform_var_name, v4 *passing_vec4) {
    GLuint target;
    target = glGetUniformLocation(shader->id, uniform_var_name);
    glUniform4fv(target, 1, (f32 *)passing_vec4);
}

internal void
set_uniform_material(Shader_Info *shader, char *uniform_var_name, Material *material) {
    size length_of_name = strlen(uniform_var_name) + 128;

    char *memory = alloca(length_of_name + 128);

    memset(memory, 0, length_of_name + 128);
    snprintf(memory, length_of_name + 128, "%s.ambient_color", uniform_var_name);
    set_uniform_vec3(shader, memory, &material->ambient_color);

    memset(memory, 0, length_of_name + 128);
    snprintf(memory, length_of_name + 128, "%s.diffuse_color", uniform_var_name);
    set_uniform_vec3(shader, memory, &material->diffuse_color);

    memset(memory, 0, length_of_name + 128);
    snprintf(memory, length_of_name + 128, "%s.specular_color", uniform_var_name);
    set_uniform_vec3(shader, memory, &material->specular_color);

    memset(memory, 0, length_of_name + 128);
    snprintf(memory, length_of_name + 128, "%s.shininess", uniform_var_name);
    set_uniform_f32(shader, memory, material->shininess);
}

internal void
set_uniform_light(Shader_Info *shader, char *uniform_var_name, Light *light) {
    size length_of_name = strlen(uniform_var_name) + 128;
    char *memory = alloca(length_of_name + 128);

    memset(memory, 0, length_of_name + 128);
    snprintf(memory, length_of_name + 128, "%s.position", uniform_var_name);
    set_uniform_vec3(shader, memory, &light->position);

    memset(memory, 0, length_of_name + 128);
    snprintf(memory, length_of_name + 128, "%s.ambient_color", uniform_var_name);
    set_uniform_vec3(shader, memory, &light->ambient_color);

    memset(memory, 0, length_of_name + 128);
    snprintf(memory, length_of_name + 128, "%s.diffuse_color", uniform_var_name);
    set_uniform_vec3(shader, memory, &light->diffuse_color);

    memset(memory, 0, length_of_name + 128);
    snprintf(memory, length_of_name + 128, "%s.specular_color", uniform_var_name);
    set_uniform_vec3(shader, memory, &light->specular_color);
}

internal void
set_uniform_f32(Shader_Info *shader, char *uniform_var_name, f32 value) {
    GLuint target;
    target = glGetUniformLocation(shader->id, uniform_var_name);

    glUniform1f(target, value);
}
