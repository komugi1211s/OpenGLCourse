
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
"    frag_c = texture(my_texture, tex_coord);\n"
"}\n";

typedef struct {
    u32 id;
} Shader_Info;

typedef struct {
    u32 id;

    i32 width;
    i32 height;
    i32 channels;
} Texture_Info;


typedef struct {
    f64 x, y;
} Mouse_Position;


typedef struct {
    Mouse_Position current_mouse;
    Mouse_Position previous_mouse;

    b32 clicking;
    Mouse_Position clicked_mouse;

    v3 camera_target;
    v3 camera_position;

} Engine_State;
