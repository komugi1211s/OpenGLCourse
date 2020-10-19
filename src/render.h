#ifndef K_RENDER
#define K_RENDER

typedef struct {
    u32 id;
} Shader_Info;

typedef struct {
    GLuint id;
    GLenum texture_type;

    i32 width;
    i32 height;
    i32 channels;
} Texture_Info;

typedef struct {
    GLuint vao;
    GLuint vbo;
} Render_Target;

internal Texture_Info load_image_file_into_texture(char *file_name, GLenum texture_type);
internal void         use_texture(Texture_Info *tex, GLenum texture_unit);

internal void         use_texture(Texture_Info *tex, GLenum texture_unit);
internal Shader_Info  initialize_shaders(const char *vtx_shader_src, const char *frag_shader_src);
internal void         use_shader(Shader_Info *shader);
internal void         set_uniform_matrix4x4(Shader_Info *shader, char *uniform_var_name, mat4x4 *passing_matrix);
internal void         set_uniform_vec3(Shader_Info *shader, char *uniform_var_name, v3 *passing_vec3);
internal void         set_uniform_vec4(Shader_Info *shader, char *uniform_var_name, v4 *passing_vec4);
#endif // K_RENDER
