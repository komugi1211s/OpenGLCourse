
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

global_variable f32 dt = 0.0f;

global_variable u32 immediate_vao_id = 0;
global_variable u32 immediate_vbo_id = 0;

global_variable u32 gizmo_vao_id = 0;
global_variable u32 gizmo_vbo_id = 0;


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
        glDeleteShader(vert_shader);
        glDeleteShader(frag_shader);

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
        glDeleteShader(vert_shader);
        glDeleteShader(frag_shader);

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
        glDeleteShader(vert_shader);
        glDeleteShader(frag_shader);

        glGetProgramInfoLog(program.id, 512, 0, error_log);
        output_error("Failed to link shader program: \n");
        output_error(error_log);

        return(program);
    }

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    glUseProgram(program.id);
    return(program);
}


internal void
gl_process_input(GLFWwindow *window, Engine_State *state) {
    state->previous_mouse = state->current_mouse;
    glfwGetCursorPos(window, &state->current_mouse.x, &state->current_mouse.y);

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (!state->clicking) {
            state->clicking = 1;
            state->clicked_mouse = state->current_mouse;
        }
    } else {
        if (state->clicking) {
            state->clicking = 0;
        }
    }

    f32 camera_speed = 2.5f * dt;

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        camera_speed = 5.0f * dt;
    } else {
        camera_speed = 2.5f * dt;
    }


    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        v3 move_towards = v3_mul_scalar(state->camera_target, camera_speed);
        state->camera_position = v3_add(&state->camera_position, &move_towards);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        v3 move_towards = v3_mul_scalar(state->camera_target, camera_speed);
        state->camera_position = v3_sub(&state->camera_position, &move_towards);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        v3 move_towards = v3_normalize(v3_cross(&state->camera_target, &state->camera_up));
        move_towards = v3_mul_scalar(move_towards, camera_speed);

        state->camera_position = v3_sub(&state->camera_position, &move_towards);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        v3 move_towards = v3_normalize(v3_cross(&state->camera_target, &state->camera_up));
        move_towards = v3_mul_scalar(move_towards, camera_speed);

        state->camera_position = v3_add(&state->camera_position, &move_towards);
    }
}

void init_immediate_stuff() {
    glGenVertexArrays(1, &immediate_vao_id);
    glGenBuffers(1, &immediate_vbo_id);

    glBindVertexArray(immediate_vao_id);


    glBindVertexArray(0);
}

void init_gizmo_stuff() {
    f32 gizmo_lines_array[] = {
        0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f,

        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
    };

    glGenVertexArrays(1, &gizmo_vao_id);
    glGenBuffers(1, &gizmo_vbo_id);

    glBindVertexArray(gizmo_vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, gizmo_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gizmo_lines_array), gizmo_lines_array, GL_STATIC_DRAW);

    glEnableVertexArrayAttrib(gizmo_vao_id, 0); // POSITION
    glEnableVertexArrayAttrib(gizmo_vao_id, 1); // COLOR

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (3 * sizeof(f32)), (void *)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (3 * sizeof(f32)), (void *)((3 * sizeof(f32)) * 6));

    glBindVertexArray(0);
}

internal void
draw_gizmo(Engine_State *engine, Shader_Info *shader) {
    glClear(GL_DEPTH_BUFFER_BIT);

    mat4x4 model = m4x4_translate(m4x4_identity(), vec_3(0.0f, 0.0f, -1.0f));

    mat4x4 perspective = m4x4_orthographic(-1.0f, 1.0f, 1.0f, -1.0f, 0.1, 100.0);

    mat4x4 view        = m4x4_look_at(engine->camera_position,
                                      v3_add(&engine->camera_position, &engine->camera_target),
                                      engine->camera_up);

    glUniformMatrix4fv(shader->model_matrix_loc, 1, GL_FALSE, (f32 *)&model);
    glUniformMatrix4fv(shader->view_matrix_loc, 1, GL_FALSE, (f32 *)&view);
    glUniformMatrix4fv(shader->projection_matrix_loc, 1, GL_FALSE, (f32 *)&perspective);

    glBindVertexArray(gizmo_vao_id);
    glDrawArrays(GL_LINES, 0, 6);
    glBindVertexArray(0);
}

int main(int argc, char **argv) {

    f32 cube_arrays[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

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

    glUseProgram(shader_program.id);
    shader_program.model_matrix_loc      = glGetUniformLocation(shader_program.id, "model");
    shader_program.view_matrix_loc       = glGetUniformLocation(shader_program.id, "view");
    shader_program.projection_matrix_loc = glGetUniformLocation(shader_program.id, "projection");

    Texture_Info tex_info;
    b32 success = load_image_into_texture("first_texture.jpg", &tex_info);
    if (!success) {
        output_error("Failed to Load a Texture\n");
        glfwTerminate();

        return(-1);
    }

    u32 vao_id;
    glGenVertexArrays(1, &vao_id);

    u32 vbo_id;
    glGenBuffers(1, &vbo_id);
    i32 ATTRIB_POSITION  = 0;
    i32 ATTRIB_TEXCOORDS = 2;

    glBindVertexArray(vao_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_arrays), cube_arrays, GL_STATIC_DRAW);
    glVertexAttribPointer(ATTRIB_POSITION,  3, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void *)0);
    glVertexAttribPointer(ATTRIB_TEXCOORDS, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void *)(3 * sizeof(f32)));

    glEnableVertexAttribArray(ATTRIB_POSITION);
    glEnableVertexAttribArray(ATTRIB_TEXCOORDS);
    glEnable(GL_DEPTH_TEST);


    Engine_State engine_state = {0};
    engine_state.camera_position = vec_3(0.0f, 0.0f,  5.0f);
    engine_state.camera_target   = vec_3(0.0f, 0.0f, -1.0f);
    engine_state.camera_up       = vec_3(0.0f, 1.0f,  0.0f);

    init_gizmo_stuff();

    f32 pitch, yaw;
    pitch = 0.0f;
    yaw = -90.0f;

    mat4x4 perspective = m4x4_perspective(to_radians_f(45.0f), 800.0f / 600.0f, 0.1, 100.0);

    f32 sensitivity = 0.1f;
    f32 last_frame = 0.0f;
    while (!glfwWindowShouldClose(game_window)) {
        f32 current_frame = glfwGetTime();
        dt = current_frame - last_frame;
        last_frame = current_frame;

        gl_process_input(game_window, &engine_state);

        if (engine_state.clicking) {
            f32 x_moved = (f32)(engine_state.current_mouse.x - engine_state.previous_mouse.x);
            f32 y_moved = (f32)(engine_state.previous_mouse.y - engine_state.current_mouse.y);

            yaw   += (x_moved * sensitivity);
            pitch += (y_moved * sensitivity);

            if (pitch > 89.0f) pitch  = 89.0f;
            if (pitch < -89.0f) pitch = -89.0f;

            f32 r_yaw = to_radians_f(yaw);
            f32 r_pitch = to_radians_f(pitch);

            v3 vector = {0};
            vector.col[0] = cosf(r_yaw) * cosf(r_pitch);
            vector.col[1] = sinf(r_pitch);
            vector.col[2] = sinf(r_yaw) * cosf(r_pitch);

            engine_state.camera_target = v3_normalize(vector);
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4x4 trans_mat   = m4x4_identity();
        mat4x4 view_mat    = m4x4_look_at(engine_state.camera_position,
                                          v3_add(&engine_state.camera_position, &engine_state.camera_target),
                                          engine_state.camera_up);

        trans_mat = m4x4_rotate_radians(&trans_mat, (float)glfwGetTime(), vec_3(1.0f, 0.0f, 0.0f));
        trans_mat = m4x4_translate(trans_mat, vec_3(1.0f, 0.0f, 0.0f));

        glUniformMatrix4fv(shader_program.model_matrix_loc,      1, GL_FALSE, (f32 *)&trans_mat);
        glUniformMatrix4fv(shader_program.view_matrix_loc,       1, GL_FALSE, (f32 *)&view_mat);
        glUniformMatrix4fv(shader_program.projection_matrix_loc, 1, GL_FALSE, (f32 *)&perspective);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex_info.id);
        glBindVertexArray(vao_id);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(cube_arrays));
        glBindVertexArray(0);
        draw_gizmo(&engine_state, &shader_program);

        glViewport(0, 0, 800, 600);

        glfwSwapBuffers(game_window);
        glfwPollEvents();
    }

    glfwTerminate();
    return(0);
}
