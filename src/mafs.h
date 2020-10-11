/*
 * Maths but for idiots like me.
 * */

#ifdef M_PI
    #define PI M_PI
#else
    #define PI 3.14159265358979323846
#endif

typedef struct {
    f32 x, y;
} v2;

// TODO: choose one of two:
//  1. find out a way to omit xyz/rgb/xyzw/rgba
//  2. give up and use c11 where anonymous struct is a thing
typedef union {
    struct { f32 x, y, z; } xyz;
    struct { f32 r, g, b; } rgb;
    f32 col[3];
} v3, v3_color;


typedef union {
    struct { f32 x, y, z, w; } xyzw;
    struct { f32 r, g, b, a; } rgba;
    f32 col[4];
} v4, v4_color;

typedef struct {
    v4 row[4];
} mat4x4;

typedef struct {
    v3 row[3];
} mat3x3;

internal inline f32
to_radians_f(f32 degrees) {
    return degrees * (PI / 180.0f);
}

internal inline f32
to_degrees_f(f32 radians) {
    return radians * (180.0f / PI);
}

internal inline v2
vec_2(f32 x, f32 y) {
    v2 result = { .x = x, .y = y };
    return result;
}

internal inline v3
vec_3(f32 x, f32 y, f32 z) {
    v3 result = {0};
    result.xyz.x = x;
    result.xyz.y = y;
    result.xyz.z = z;
    return result;
}

internal inline v4
vec_4(f32 x, f32 y, f32 z, f32 w) {
    v4 result;
    result.xyzw.x = x;
    result.xyzw.y = y;
    result.xyzw.z = z;
    result.xyzw.w = w;
    return result;
}

internal inline mat4x4
m4x4_identity() {
    mat4x4 result = {0};

    result.row[0].col[0] = 1.0f;
    result.row[1].col[1] = 1.0f;
    result.row[2].col[2] = 1.0f;
    result.row[3].col[3] = 1.0f;

    return result;
}

internal inline mat4x4
m4x4_orthographic(f32 start_x, f32 width,
                  f32 start_y, f32 height,
                  f32 near_plane, f32 far_plane)
{
    return m4x4_identity();
}

internal inline mat4x4
m4x4_perspective(f32 field_of_view, f32 aspect_ratio,
                 f32 near_plane,    f32 far_plane)
{
    return m4x4_identity();
}

internal inline f32
v3_len(v3 *target_p) {
    f32 x = (target_p->xyz.x * target_p->xyz.x);
    f32 y = (target_p->xyz.y * target_p->xyz.y);
    f32 z = (target_p->xyz.z * target_p->xyz.z);

    return sqrtf(x + y + z);
}

internal inline v3
v3_normalize(v3 target) {
    f32 length = v3_len(&target);

    target.xyz.x /= length;
    target.xyz.y /= length;
    target.xyz.z /= length;

    return target;
}

internal inline v3
v3_mul_scalar(v3 target, f32 scalar) {
    target.xyz.x *= scalar;
    target.xyz.y *= scalar;
    target.xyz.z *= scalar;

    return target;
}

internal inline v4
v4_mul_scalar(v4 target, f32 scalar) {
    target.xyzw.x *= scalar;
    target.xyzw.y *= scalar;
    target.xyzw.z *= scalar;
    target.xyzw.w *= scalar;

    return target;
}

// TODO: bug-prone, could overflow
internal f32
v4_to_scalar(v4 *target) {
    return (target->xyzw.x
            + target->xyzw.y
            + target->xyzw.z
            + target->xyzw.w);
}

// ASSERT: col must be 0 ~ 3
internal v4
m4x4_get_col(mat4x4 *target, i32 col) {
    col *= 4;
    f32 *t = (f32 *)target;

    return vec_4(t[col], t[col + 1], t[col + 2], t[col + 3]);
}

internal mat4x4
m4x4_mul(mat4x4 *left, mat4x4 *right) {
    mat4x4 out = {0};

    // TODO: This is hella scary
    for (i32 row = 0; row < 4; row++) {
        for (i32 col = 0; col < 4; col++) {
            out.row[row].col[col]  = (left->row[row].col[0] * right->row[0].col[col])
                                   + (left->row[row].col[1] * right->row[1].col[col])
                                   + (left->row[row].col[2] * right->row[2].col[col])
                                   + (left->row[row].col[3] * right->row[3].col[col]);
        }
    }

    return out;
}

// TODO: Gimbal Lock Problem
// super slow operation
// still don't know what I'm doing
internal mat4x4
m4x4_rotate_radians(mat4x4 *mat, f32 radians, v3 axis) {
    f32 c = cosf(radians);
    f32 s = sinf(radians);

    f32 c_neg = (1.0f - c);
    axis = v3_normalize(axis);

    mat4x4 rot_vec = m4x4_identity();

    rot_vec.row[0].col[0] = axis.xyz.x * axis.xyz.x * (1 - c) + c;
    rot_vec.row[0].col[1] = axis.xyz.x * axis.xyz.y * (1 - c) - axis.xyz.z * s;
    rot_vec.row[0].col[2] = axis.xyz.x * axis.xyz.z * (1 - c) + axis.xyz.y * s;
    rot_vec.row[0].col[3] = 0;

    rot_vec.row[1].col[0] = axis.xyz.y * axis.xyz.x * (1 - c) + axis.xyz.z * s;
    rot_vec.row[1].col[1] = axis.xyz.y * axis.xyz.y * (1 - c) + c;
    rot_vec.row[1].col[2] = axis.xyz.y * axis.xyz.z * (1 - c) - axis.xyz.x * s;
    rot_vec.row[1].col[3] = 0;

    rot_vec.row[2].col[0] = axis.xyz.z * axis.xyz.x * (1 - c) - axis.xyz.y * s;
    rot_vec.row[2].col[1] = axis.xyz.z * axis.xyz.y * (1 - c) + axis.xyz.x * s;
    rot_vec.row[2].col[2] = axis.xyz.z * axis.xyz.z * (1 - c) + c;
    rot_vec.row[2].col[3] = 0;

    rot_vec.row[3].col[0] = 0;
    rot_vec.row[3].col[1] = 0;
    rot_vec.row[3].col[2] = 0;
    rot_vec.row[3].col[3] = 1;

    return m4x4_mul(mat, &rot_vec);
}

internal inline mat4x4
m4x4_rotate_degrees(mat4x4 *mat, f32 degrees, v3 axis) {
    return m4x4_rotate_radians(mat, to_radians_f(degrees), axis);
}

