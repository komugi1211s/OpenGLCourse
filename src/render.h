#ifndef K_RENDER
#define K_RENDER

global_variable const char *VERTEX_SHADER = "#version 330 core\n"
"\n"
"layout (location = 0) in vec3 vec_position; \n"
"layout (location = 1) in vec3 vec_color;\n"
"layout (location = 2) in vec2 in_tex_coords; \n"
"out vec4 passed_color; \n"
"out vec2 tex_coord; \n"
"uniform mat4 model; \n"
"uniform mat4 view; \n"
"uniform mat4 projection; \n"
"void main() \n"
"{ \n"
"    gl_Position = projection * view * model * vec4(vec_position, 1.0); \n"
"    passed_color = vec4(vec_color, 1.0); \n"
"    tex_coord = in_tex_coords; \n"
"} \n";

global_variable const char *FRAGMENT_SHADER = "#version 330 core \n"
"out vec4 frag_c; \n"
"in  vec4 passed_color; \n"
"in  vec2 tex_coord; \n"
"uniform sampler2D my_texture;\n"
"\n"
"void main() \n"
"{\n"
"    frag_c = texture(my_texture, tex_coord) * passed_color;\n"
"}\n";

typedef struct {
    u32 id;

    u32 model_matrix_loc;
    u32 view_matrix_loc;
    u32 projection_matrix_loc;
} Shader_Info;

typedef struct {
    GLuint id;
    GLenum texture_type;

    i32 width;
    i32 height;
    i32 channels;
} Texture_Info;

typedef struct {
    GLuint vao_id;
    GLuint vbo_id;
    GLuint ssbo_id;
} Render_Target;

internal Texture_Info load_image_file_into_texture(char *file_name, GLenum texture_type);
internal void         use_texture(Texture_Info *tex);

internal Shader_Info  initialize_shaders(const char *vtx_shader_src, const char *frag_shader_src);
internal void         use_shader(Shader_Info *shader);
internal void         set_uniform_matrix4x4(Shader_Info *shader, char *uniform_var_name, mat4x4 *passing_matrix);
internal void         set_uniform_vec3(Shader_Info *shader, char *uniform_var_name, v3 *passing_vec3);
internal void         set_uniform_vec4(Shader_Info *shader, char *uniform_var_name, v4 *passing_vec4);
#endif // K_RENDER
