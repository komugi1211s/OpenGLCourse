
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

#include "mafs.h"
#include "render.h"

#include "main.h"

global_variable f32 dt = 0.0f;

global_variable GLuint gizmo_vao_id = 0;
global_variable GLuint gizmo_vbo_id = 0;

const f32 gizmo_lines_array[] = {
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

// TODO: more robustness
internal void
output_error(char *message) {
#if _MSC_VER
    OutputDebugString(message);
#else
    fprintf(stderr, "%s", message);
#endif
}

#if _MSC_VER
#define fz_allocate(AllocSize) VirtualAlloc(0, AllocSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE)
#define fz_free(FreeTarget)    VirtualFree(FreeTarget)
#else
#define fz_allocate(AllocSize) malloc(AllocSize)
#define fz_free(FreeTarget)    free(FreeTarget)
#endif


// TODO: this assumes that it everything never fails.
internal char *
allocate_and_read_entire_file(char *filename) {
    FILE *fp = 0;
    fp = fopen(filename, "r");
    Assert(fp);

    Assert(0 == fseek(fp, 0, SEEK_END));
    size_t file_length = ftell(fp);
    Assert(0 == fseek(fp, 0, SEEK_SET));

    char *file_memory = fz_allocate(file_length + 1);
    Assert(file_memory);

    u32 bytes_read = fread(file_memory, 1, file_length, fp);
    file_memory[bytes_read] = '\0';

    fclose(fp);
    return file_memory;
}

internal void
gl_process_input(GLFWwindow *window, Engine_State *state, v3 *light_pos) {
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

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        light_pos->xyz.x += 0.1;
        light_pos->xyz.z += 0.1;
    }

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        light_pos->xyz.x -= 0.1;
        light_pos->xyz.z -= 0.1;
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

void init_gizmo_stuff() {
    glGenVertexArrays(1, &gizmo_vao_id);
    glGenBuffers(1, &gizmo_vbo_id);

    glBindVertexArray(gizmo_vao_id);
    glBindBuffer(GL_ARRAY_BUFFER, gizmo_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gizmo_lines_array), gizmo_lines_array, GL_STATIC_DRAW);

    glEnableVertexArrayAttrib(gizmo_vao_id, 0); // POSITION
    glEnableVertexArrayAttrib(gizmo_vao_id, 1); // COLOR

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (3 * sizeof(f32)), (void *)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (3 * sizeof(f32)), (void *)((3 * sizeof(f32)) * 6));

    glBindVertexArray(0);
}

internal void
draw_gizmo(Engine_State *engine, Shader_Info *gizmo_shader) {
    glViewport(0, 0, 100, 100);
    glClear(GL_DEPTH_BUFFER_BIT);

    mat4x4 model = m4x4_identity();
    mat4x4 view  = m4x4_look_at(engine->camera_position,
                                      v3_add(&engine->camera_position, &engine->camera_target),
                                      engine->camera_up);

    view = m4x4_translate(view, vec_3(0.0f, -0.05f, -3.0f));
    mat4x4 ortho = m4x4_orthographic(-1.0f, 1.0f, 1.0f, -1.0f, 0.1, 100.0);
    glBindVertexArray(gizmo_vao_id);

    /*
     * Set Vertex Shader Variable.
     */
    set_uniform_matrix4x4(gizmo_shader, "model", &model);
    set_uniform_matrix4x4(gizmo_shader, "view", &view);
    set_uniform_matrix4x4(gizmo_shader, "projection", &ortho);

    glDrawArrays(GL_LINES, 0, 6);
    glBindVertexArray(0);
    glViewport(0, 0, 800, 600);
}

internal void
draw_origin_point(Engine_State *state, Shader_Info *shader) {

    mat4x4 perspective = m4x4_perspective(to_radians_f(90.0f), 800.0f / 600.0f, 0.1, 1000.0);
    mat4x4 trans_mat   = m4x4_identity();
    mat4x4 view_mat    = m4x4_look_at(state->camera_position,
                                      v3_add(&state->camera_position, &state->camera_target),
                                      state->camera_up);

    /*
     * Set Vertex Shader Variable.
     */
    set_uniform_matrix4x4(shader, "view", &view_mat);
    set_uniform_matrix4x4(shader, "model", &trans_mat);
    set_uniform_matrix4x4(shader, "projection", &perspective);
    glBindVertexArray(gizmo_vao_id);
    glDrawArrays(GL_LINES, 0, 6);
    glBindVertexArray(0);
}


int main(int argc, char **argv) {

    f32 cube_arrays[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    Assert(glfwInit());

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *game_window = glfwCreateWindow(800, 600, "OpenGL", 0, 0);
    Assert(game_window);

    glfwMakeContextCurrent(game_window);

    GLenum glew_result = glewInit();
    Assert(glew_result == GLEW_OK);

    glViewport(0, 0, 800, 600);


    /*
     * Set up the shader.
     * */
    char *vertex_shader_file = allocate_and_read_entire_file("./render.vert");
    char *object_frag_file   = allocate_and_read_entire_file("./object_render.frag");
    char *light_frag_file    = allocate_and_read_entire_file("./light_render.frag");
    char *gizmo_frag_file    = allocate_and_read_entire_file("./gizmo_render.frag");

    Shader_Info object_shader = {0};
    Shader_Info light_shader  = {0};
    Shader_Info gizmo_shader  = {0};

    object_shader = initialize_shaders(vertex_shader_file, object_frag_file);
    Assert(object_shader.id);

    light_shader  = initialize_shaders(vertex_shader_file, light_frag_file);
    Assert(light_shader.id);

    gizmo_shader  = initialize_shaders(vertex_shader_file, gizmo_frag_file);
    Assert(gizmo_shader.id);

    fz_free(vertex_shader_file);
    fz_free(object_frag_file);
    fz_free(light_frag_file);
    fz_free(gizmo_frag_file);

    /*
     * Initialize Gizmo.
     * */
    init_gizmo_stuff();

    /*
     * Load Texture.
     * */
    Texture_Info tex_info;
    tex_info = load_image_file_into_texture("first_texture.jpg", GL_TEXTURE_2D);
    Assert(tex_info.width > 0 && tex_info.height > 0);

    Render_Target cube = {0};
    Render_Target light_source = {0};

    glGenVertexArrays(1, &cube.vao);
    glGenBuffers(1, &cube.vbo);

    //NOTE(fuzzy): Cleanup
    i32 ATTRIB_POSITION  = 0;
    i32 ATTRIB_NORMAL    = 2;

    // Set up the cube.
    glBindVertexArray(cube.vao);
    glBindBuffer(GL_ARRAY_BUFFER, cube.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_arrays), cube_arrays, GL_STATIC_DRAW);

    glEnableVertexArrayAttrib(cube.vao, ATTRIB_POSITION);
    glEnableVertexArrayAttrib(cube.vao, ATTRIB_NORMAL);
    glVertexAttribPointer(ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(f32), (void *)0);
    glVertexAttribPointer(ATTRIB_NORMAL,   3, GL_FLOAT, GL_FALSE, 6 * sizeof(f32), (void *)(3 * sizeof(f32)));
    glBindVertexArray(0);


    // Set up the light source.
    glGenVertexArrays(1, &light_source.vao);
    glBindVertexArray(light_source.vao);

    light_source.vbo = cube.vbo; // Same Vertex Buffer.

    glEnableVertexArrayAttrib(light_source.vao, ATTRIB_POSITION);
    glVertexAttribPointer(ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(f32), (void *)0);

    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);

    /*
     * Setup the engine.
     * */
    Engine_State engine_state = {0};
    engine_state.camera_position = vec_3(0.0f, 0.0f,  5.0f);
    engine_state.camera_target   = vec_3(0.0f, 0.0f, -1.0f);
    engine_state.camera_up       = vec_3(0.0f, 1.0f,  0.0f);
    // engine_state.imm_gui         = imm_initialize();

    f32 pitch, yaw;
    pitch = 0.0f;
    yaw = -90.0f;

    mat4x4 perspective = m4x4_perspective(to_radians_f(90.0f), 800.0f / 600.0f, 0.1, 1000.0);

    f32 sensitivity = 0.1f;
    f32 last_frame = 0.0f;
    v3 light_pos = vec_3(3.0f, 3.0f, -2.0f);

    while (!glfwWindowShouldClose(game_window)) {
        f32 current_frame = glfwGetTime();
        dt = current_frame - last_frame;
        last_frame = current_frame;

        gl_process_input(game_window, &engine_state, &light_pos);

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

        mat4x4 view_mat = m4x4_look_at(engine_state.camera_position,
                                       v3_add(&engine_state.camera_position, &engine_state.camera_target),
                                       engine_state.camera_up);

        // Drawing default cube.
        {
            mat4x4 trans_mat = m4x4_scale(m4x4_identity(), vec_3(10.0f, 10.0f, 1.0f));
            v3 object_color = vec_3(1.0f, 0.5f, 0.31f);
            v3 light_color = vec_3(1.0f, 1.0f, 1.0f);

            use_shader(&object_shader);

            /*
             * Set Vertex Shader Variable.
             */
            set_uniform_matrix4x4(&object_shader, "view", &view_mat);
            set_uniform_matrix4x4(&object_shader, "model", &trans_mat);
            set_uniform_matrix4x4(&object_shader, "projection", &perspective);

            set_uniform_vec3(&object_shader, "light_position",  &light_pos);

            /*
             * Set Fragment Shader Variable.
             */
            set_uniform_vec3(&object_shader, "ObjectColor", &object_color);
            set_uniform_vec3(&object_shader, "LightColor",  &light_color);

            use_texture(&tex_info, GL_TEXTURE0);
            glBindVertexArray(cube.vao);
            glDrawArrays(GL_TRIANGLES, 0, sizeof(cube_arrays));
            glBindVertexArray(0);
        }

        // Drawing Light Source Cube.
        {
            mat4x4 model_mat = m4x4_translate(m4x4_identity(), light_pos);

            use_shader(&light_shader);

            /*
             * Set Vertex Shader Variable.
             */
            set_uniform_matrix4x4(&light_shader, "model", &model_mat);
            set_uniform_matrix4x4(&light_shader, "view", &view_mat);
            set_uniform_matrix4x4(&light_shader, "projection", &perspective);

            glBindVertexArray(light_source.vao);
            glDrawArrays(GL_TRIANGLES, 0, sizeof(cube_arrays));
            glBindVertexArray(0);
        }

        // Drawing gizmos'n stuff.
        {
            use_shader(&gizmo_shader);
            draw_origin_point(&engine_state, &gizmo_shader);
            draw_gizmo(&engine_state, &gizmo_shader);
        }

        glViewport(0, 0, 800, 600);

        glfwSwapBuffers(game_window);
        glfwPollEvents();
    }

    glfwTerminate();
    return(0);
}
