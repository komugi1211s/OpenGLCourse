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

typedef struct {
    v3 ambient_color;
    v3 diffuse_color;
    v3 specular_color;
    f32 shininess;
} Material;

typedef struct {
    v3 position;
    v3 ambient_color;
    v3 diffuse_color;
    v3 specular_color;
} Light;


internal Texture_Info load_image_file_into_texture(char *file_name, GLenum texture_type);
internal void         use_texture(Texture_Info *tex, GLenum texture_unit);

internal void         use_texture(Texture_Info *tex, GLenum texture_unit);
internal Shader_Info  initialize_shaders(const char *vtx_shader_src, const char *frag_shader_src);
internal void         use_shader(Shader_Info *shader);
internal void         set_uniform_f32(Shader_Info *shader, char *uniform_var_name, f32 value);
internal void         set_uniform_matrix4x4(Shader_Info *shader, char *uniform_var_name, mat4x4 *passing_matrix);
internal void         set_uniform_vec3(Shader_Info *shader, char *uniform_var_name, v3 *passing_vec3);
internal void         set_uniform_vec4(Shader_Info *shader, char *uniform_var_name, v4 *passing_vec4);
internal void         set_uniform_material(Shader_Info *shader, char *uniform_var_name, Material *material);
internal void         set_uniform_light(Shader_Info *shader, char *uniform_var_name, Light *light);
#endif // K_RENDER
