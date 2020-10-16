
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
    v3 camera_up;
} Engine_State;
