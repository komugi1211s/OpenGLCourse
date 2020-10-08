
#include "toolbox.h"
#if _MSC_VER
#include <windows.h>
#endif
#include <stdarg.h>

// Because I need to specify this for windows.
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <math.h> // TODO: Some day i'll blow this line off out of existence.

global_variable const char *VERTEX_SHADER = "#version 330 core\n"
"\n"
"layout (location = 0) in vec3 vec_position; \n"
"layout (location = 1) in vec3 vec_color; \n"
"out vec4 passed_color;\n"
"void main() \n"
"{ \n"
"    gl_Position = vec4(vec_position, 1.0); \n"
"    passed_color = vec4(vec_color, 1.0); \n"
"} \n";

global_variable const char *FRAGMENT_SHADER = "#version 330 core \n"
"out vec4 frag_c; \n"
"in  vec4 passed_color; \n"
"\n"
"void main() \n"
"{\n"
"    frag_c = passed_color;\n"
"}\n";

// TODO: more robustness
internal void
output_error(char *message) {
#if _MSC_VER
    OutputDebugString(message);
#else
    fprintf(stderr, "%s", message);
#endif
}

internal u32
initialize_shaders(const char *vtx_shader_src, const char *frag_shader_src) {
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
        return(0);
    }
    success = 0;

    glShaderSource(frag_shader, 1, &frag_shader_src, 0);
    glCompileShader(frag_shader);

    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(frag_shader, 512, 0, error_log);
        output_error("Failed to Compile fragment shader: \n");
        output_error(error_log);
        return(0);
    }
    success = 0;

    u32 program = glCreateProgram();
    glAttachShader(program, vert_shader);
    glAttachShader(program, frag_shader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, 0, error_log);
        output_error("Failed to link shader program: \n");
        output_error(error_log);
        return(0);
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
        -0.5f,  0.0f, 0.0f,
         0.5f,  0.0f, 0.0f,
         0.0f,  0.5f, 0.0f,
        // COLOR
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f
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

    u32 shader_program = initialize_shaders(VERTEX_SHADER, FRAGMENT_SHADER);
    if (!shader_program) {
        output_error("Failed to initialize shader\n");
        return(-1);
    }

    u32 vao_id;
    glGenVertexArrays(1, &vao_id);

    u32 triangle_vbo_id;
    glGenBuffers(1, &triangle_vbo_id);

    i32 ATTRIB_POSITION = 0;
    i32 ATTRIB_COLOR    = 1;

    glBindVertexArray(vao_id);
    glBindBuffer(GL_ARRAY_BUFFER, triangle_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_arrays), triangle_arrays, GL_STATIC_DRAW);

    glVertexAttribPointer(ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void *)0);
    glVertexAttribPointer(ATTRIB_COLOR,    3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void *)(3 * 3 * sizeof(f32)));
    glEnableVertexAttribArray(ATTRIB_POSITION);
    glEnableVertexAttribArray(ATTRIB_COLOR);

    i32 vertex_color_loc = glGetUniformLocation(shader_program, "vertex_color");

    while (!glfwWindowShouldClose(game_window)) {
        gl_process_input(game_window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program);

        glBindVertexArray(vao_id);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        glfwSwapBuffers(game_window);
        glfwPollEvents();
    }

    glfwTerminate();
    return(0);
}
