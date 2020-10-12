
/*
 * TODO:
 *  Platform Layer ::
 *      Audio
 *      File System
 *      Input Event
 *      Memory Management
 *      Threading (eh, maybe.)
 *      Hot Reloading
 *
 *  Some Imported Library Cleanup ::
 *      stb_image.h (should load png / jpg on my own, for learning reason!)
 *      math.h      (I guess I can't remove this? maybe? I'd much rather write on my own, if Sine function isn't that complicated!)
 * */


#include "toolbox.h"
#if _MSC_VER
#include <windows.h>
#endif
#include <stdarg.h>

// Because I need to specify this for windows.
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" // TODO: Some day i'll blow this line off out of existence.
#include <math.h>      // TODO: Some day i'll blow this line off out of existence.
#include "mafs.h"

#include "main.h"


// TODO: more robustness
internal void
output_error(char *message) {
#if _MSC_VER
    OutputDebugString(message);
#else
    fprintf(stderr, "%s", message);
#endif
}

internal b32
load_image_into_texture(char *image_name, Texture_Info *out_tex_info) {
    u32 id;
    i32 width, height, channels;

    u8 *file_data = stbi_load(image_name, &width, &height, &channels, 0);
    if (!file_data) return(-1);

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, file_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(file_data);

    out_tex_info->id = id;
    out_tex_info->width = width;
    out_tex_info->height = height;
    out_tex_info->channels = channels;
    return(1);
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
        output_error("Failed to Compile vertex shader: \n");
        output_error(error_log);
        return(program);
    }
    success = 0;

    glShaderSource(frag_shader, 1, &frag_shader_src, 0);
    glCompileShader(frag_shader);

    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(frag_shader, 512, 0, error_log);
        output_error("Failed to Compile fragment shader: \n");
        output_error(error_log);
        return(program);
    }
    success = 0;

    program.id  = glCreateProgram();
    glAttachShader(program.id, vert_shader);
    glAttachShader(program.id, frag_shader);
    glLinkProgram(program.id);

    glGetProgramiv(program.id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program.id, 512, 0, error_log);
        output_error("Failed to link shader program: \n");
        output_error(error_log);
        return(program);
    }

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    return(program);
}


internal void
gl_process_input(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);
}

int main(int argc, char **argv) {
    f32 triangle_arrays[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, 0.5f,  0.0f,
         -0.5f, 0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,

        // Tex Coord
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
    };

    u32 indexing_arrays[] = {
        0, 2, 1,
        1, 3, 0
    };

    if (!glfwInit()) {
        output_error("Failed to initialize GLFW.\n");
        return(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *game_window = glfwCreateWindow(800, 600, "OpenGL", 0, 0);
    if (!game_window) {
        output_error("Failed to create opengl window\n");
        glfwTerminate();
        return(-1);
    }

    glfwMakeContextCurrent(game_window);

    if (glewInit() != GLEW_OK) {
        output_error("Failed to initialize GLEW\n");
        glfwTerminate();
        return(-1);
    }

    glViewport(0, 0, 800, 600);
    Shader_Info shader_program = initialize_shaders(VERTEX_SHADER, FRAGMENT_SHADER);
    if (!shader_program.id) {
        output_error("Failed to initialize shader\n");
        glfwTerminate();

        return(-1);
    }

    Texture_Info tex_info;
    b32 success = load_image_into_texture("first_texture.jpg", &tex_info);
    if (!success) {
        output_error("Failed to Load a Texture\n");
        glfwTerminate();

        return(-1);
    }

    u32 vao_id;
    glGenVertexArrays(1, &vao_id);

    u32 vbo_id, ebo_id;
    glGenBuffers(1, &vbo_id);
    glGenBuffers(1, &ebo_id);

    i32 ATTRIB_POSITION  = 0;
    i32 ATTRIB_TEXCOORDS = 2;

    glBindVertexArray(vao_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_arrays), triangle_arrays, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexing_arrays), indexing_arrays, GL_STATIC_DRAW);

    glVertexAttribPointer(ATTRIB_POSITION,  3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void *)0);
    glVertexAttribPointer(ATTRIB_TEXCOORDS, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(f32), (void *)(4 * 3 * sizeof(f32)));

    glEnableVertexAttribArray(ATTRIB_POSITION);
    glEnableVertexAttribArray(ATTRIB_TEXCOORDS);

    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(game_window)) {
        gl_process_input(game_window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader_program.id);
        u32 model_loc = glGetUniformLocation(shader_program.id, "model");
        u32 view_loc = glGetUniformLocation(shader_program.id, "view");
        u32 projection_loc = glGetUniformLocation(shader_program.id, "projection");

        mat4x4 trans_mat = m4x4_identity();
        mat4x4 view_mat = m4x4_identity();
        mat4x4 perspective = m4x4_perspective(to_radians_f(45.0f), 800.0f / 600.0f, 0.1, 100.0);

        trans_mat = m4x4_rotate_radians(&trans_mat, (float)glfwGetTime(), vec_3(1.0f, 0.0f, 1.0f));
        view_mat = m4x4_translate(view_mat, vec_3(0.0f, 0.0f, -5.0f));

        glUniformMatrix4fv(model_loc,      1, GL_FALSE, (f32 *)&trans_mat);
        glUniformMatrix4fv(view_loc,       1, GL_FALSE, (f32 *)&view_mat);
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (f32 *)&perspective);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex_info.id);
        glBindVertexArray(vao_id);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glViewport(0, 0, 800, 600);

        glfwSwapBuffers(game_window);
        glfwPollEvents();
    }

    glfwTerminate();
    return(0);
}
