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

internal inline f32
v3_dot(v3 *first, v3 *second) {
    return  (first->col[0] * second->col[0])
          + (first->col[1] * second->col[1])
          + (first->col[2] * second->col[2]);
}

internal inline v3
v3_cross(v3 *first, v3 *second) {
    v3 result = {0};

    result.col[0] = (first->col[1] * second->col[2]) - (first->col[2] * second->col[1]);
    result.col[1] = (first->col[2] * second->col[0]) - (first->col[0] * second->col[2]);
    result.col[2] = (first->col[0] * second->col[1]) - (first->col[1] * second->col[0]);
    return result;
}

internal inline f32
v4_dot(v4 *first, v4 *second) {
    return  (first->col[0] * second->col[0])
          + (first->col[1] * second->col[1])
          + (first->col[2] * second->col[2])
          + (first->col[3] * second->col[3]);
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
m4x4_scale(mat4x4 target, v3 new_size) {
    target.row[0].col[0] = new_size.col[0];
    target.row[1].col[1] = new_size.col[1];
    target.row[2].col[2] = new_size.col[2];

    return target;
}

// Because Windows is awful and can't name variables "near" and "far"!!!
internal inline mat4x4
m4x4_orthographic(f32 left, f32 right,
                  f32 top, f32 bottom,
                  f32 z_near, f32 z_far) {
    mat4x4 out = {0};

    out.row[0].col[0] = 2.0f / (right - left);
    out.row[1].col[1] = 2.0f / (top - bottom);
    out.row[2].col[2] = -2.0f / (z_far - z_near);
    out.row[3].col[3] = 1.0f;

    out.row[3].col[0] = -(right + left) / (right - left);
    out.row[3].col[1] = -(top + bottom) / (top - bottom);
    out.row[3].col[2] = -(z_far + z_near) / (z_far - z_near);

    return out;
}

/*
 * Left Handed Perspective.
 */
internal inline mat4x4
m4x4_perspective(f32 field_of_view, f32 aspect_ratio,
                 f32 z_near,    f32 z_far)
{
    mat4x4 perspective = {0};
    f32 tan_fov = tanf(field_of_view / 2.0);

    perspective.row[0].col[0] = 1.0f / (aspect_ratio * tan_fov);
    perspective.row[1].col[1] = 1.0f / tan_fov;
    perspective.row[2].col[3] = -1.0f;
    perspective.row[2].col[2] = (z_near + z_far) / (z_near - z_far);
    perspective.row[3].col[2] = (2.0f * z_far * z_near) / (z_near - z_far);

    return perspective;
}

internal inline f32
v3_len(v3 *target_p) {
    f32 x = (target_p->xyz.x * target_p->xyz.x);
    f32 y = (target_p->xyz.y * target_p->xyz.y);
    f32 z = (target_p->xyz.z * target_p->xyz.z);

    return sqrtf(x + y + z);
}

internal inline v3
v3_add(v3 *target, v3 *adder) {
    v3 result = {0};

    result.xyz.x = target->xyz.x + adder->xyz.x;
    result.xyz.y = target->xyz.y + adder->xyz.y;
    result.xyz.z = target->xyz.z + adder->xyz.z;

    return result;
}

internal inline v3
v3_sub(v3 *target, v3 *subtractor) {
    v3 result = {0};

    result.xyz.x = target->xyz.x - subtractor->xyz.x;
    result.xyz.y = target->xyz.y - subtractor->xyz.y;
    result.xyz.z = target->xyz.z - subtractor->xyz.z;

    return result;
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

    for (i32 i = 0; i < 4; i++) {
        for (i32 j = 0; j < 4; j++) {
            out.row[i].col[j]  = (left->row[i].col[0] * right->row[0].col[j])
                               + (left->row[i].col[1] * right->row[1].col[j])
                               + (left->row[i].col[2] * right->row[2].col[j])
                               + (left->row[i].col[3] * right->row[3].col[j]);
        }
    }

    return out;
}

// Column Major Translation.
internal mat4x4
m4x4_translate(mat4x4 original, v3 translate_by) {
    original.row[3].col[0] = translate_by.col[0];
    original.row[3].col[1] = translate_by.col[1];
    original.row[3].col[2] = translate_by.col[2];
    return original;
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

internal inline mat4x4
m4x4_look_at(v3 position, v3 target, v3 up) {
    mat4x4 look_at_matrix = {0};

    v3 direction = v3_normalize(v3_sub(&position, &target));
    v3 right = v3_normalize(v3_cross(&up, &direction));
    up = v3_cross(&direction, &right);

    look_at_matrix.row[0].col[0] = right.col[0];
    look_at_matrix.row[1].col[0] = right.col[1];
    look_at_matrix.row[2].col[0] = right.col[2];

    look_at_matrix.row[0].col[1] = up.col[0];
    look_at_matrix.row[1].col[1] = up.col[1];
    look_at_matrix.row[2].col[1] = up.col[2];

    look_at_matrix.row[0].col[2] = direction.col[0];
    look_at_matrix.row[1].col[2] = direction.col[1];
    look_at_matrix.row[2].col[2] = direction.col[2];

    look_at_matrix.row[3].col[3] = 1.0f;

    // Basically this works because 
    // the calculation for position (3, 0) is done by
    // (right[0] * -position[0]) + (right[1] * -position[1]) + ...
    // which is just really a dot product of two.
    // but since you're getting a dot product of POSITIVE position and it'll return the opposite value,
    // you negate the result of dot product in the end
    look_at_matrix.row[3].col[0] = -v3_dot(&right, &position);
    look_at_matrix.row[3].col[1] = -v3_dot(&up,    &position);
    look_at_matrix.row[3].col[2] = -v3_dot(&direction, &position);
    return look_at_matrix;
}

