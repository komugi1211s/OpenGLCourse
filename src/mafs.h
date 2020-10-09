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
} v3, v3_color;

typedef union {
    struct { f32 x, y, z, w; } xyzw;
    struct { f32 r, g, b, a; } rgba;
} v4, v4_color;

typedef struct {
    v4 x, y, z, w;
} mat4x4;

typedef struct {
    v3 x, y, z;
} mat3x3;

f32 to_radians_f(f32 degrees) {
    return degrees * (PI / 180.0f);
}

f32 to_degrees_f(f32 radians) {
    return radians * (180.0f / PI);
}

internal v2
vec_2(f32 x, f32 y) {
    v2 result = { .x = x, .y = y };
    return result;
}

internal v3
vec_3(f32 x, f32 y, f32 z) {
    v3 result = {0};
    result.xyz.x = x;
    result.xyz.y = y;
    result.xyz.z = z;
    return result;
}

internal v4
vec_4(f32 x, f32 y, f32 z, f32 w) {
    v4 result;
    result.xyzw.x = x;
    result.xyzw.y = y;
    result.xyzw.z = z;
    result.xyzw.w = w;
    return result;
}

internal mat4x4
m4x4_identity() {
    mat4x4 result = {0};

    result.x.xyzw.x = 1.0f;
    result.y.xyzw.y = 1.0f;
    result.z.xyzw.z = 1.0f;
    result.w.xyzw.w = 1.0f;

    return result;
}

internal f32
v3_len(v3 *target_p) {
    f32 x = (target_p->xyz.x * target_p->xyz.x);
    f32 y = (target_p->xyz.y * target_p->xyz.y);
    f32 z = (target_p->xyz.z * target_p->xyz.z);

    return sqrtf(x + y + z);
}

internal v3
v3_normalize(v3 target) {
    f32 length = v3_len(&target);

    target.xyz.x /= length;
    target.xyz.y /= length;
    target.xyz.z /= length;

    return target;
}

internal v3
v3_mul_scalar(v3 target, f32 scalar) {
    target.xyz.x *= scalar;
    target.xyz.y *= scalar;
    target.xyz.z *= scalar;

    return target;
}


internal void
ptr_m4x4_mul(mat4x4 *left, mat4x4 *right, mat4x4 *out) {
}

internal mat4x4
m4x4_mul(mat4x4 left, mat4x4 right) {
    mat4x4 out;
    ptr_m4x4_mul(&left, &right, &out);
    return out;
}

// TODO: Gimbal Lock Problem
// super slow operation
// still don't know what I'm doing
void m4x4_rotate_radians(mat4x4 *mat, f32 radians, v3 axis) {
    f32 c = cosf(radians);
    f32 s = sinf(radians);

    f32 c_neg = (1.0f - c);
    axis = v3_normalize(axis);

    mat->x.xyzw.x = axis.xyz.x * axis.xyz.x * (1 - c) + c;
    mat->x.xyzw.y = axis.xyz.x * axis.xyz.y * (1 - c) - axis.xyz.z * s;
    mat->x.xyzw.z = axis.xyz.x * axis.xyz.z * (1 - c) + axis.xyz.y * s;
    mat->x.xyzw.w = 0;

    mat->y.xyzw.x = axis.xyz.y * axis.xyz.x * (1 - c) + axis.xyz.z * s;
    mat->y.xyzw.y = axis.xyz.y * axis.xyz.y * (1 - c) + c;
    mat->y.xyzw.z = axis.xyz.y * axis.xyz.z * (1 - c) - axis.xyz.x * s;
    mat->y.xyzw.w = 0;

    mat->z.xyzw.x = axis.xyz.z * axis.xyz.x * (1 - c) - axis.xyz.y * s;
    mat->z.xyzw.y = axis.xyz.z * axis.xyz.y * (1 - c) + axis.xyz.x * s;
    mat->z.xyzw.z = axis.xyz.z * axis.xyz.z * (1 - c) + c;
    mat->z.xyzw.w = 0;

    mat->w.xyzw.x = 0;
    mat->w.xyzw.y = 0;
    mat->w.xyzw.z = 0;
    mat->w.xyzw.w = 1;
}

void m4x4_rotate_degrees(mat4x4 *mat, f32 degrees, v3 axis) {
    m4x4_rotate_radians(mat, to_radians_f(degrees), axis);
}
