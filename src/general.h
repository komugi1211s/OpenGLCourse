#ifndef K_UTILITY_
#define K_UTILITY_
#include <stdint.h>

#define internal        static
#define global_variable static

typedef uint8_t  b8;
typedef uint16_t b16;
typedef uint32_t b32;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float  f32;
typedef double f64;

typedef size_t    size;
typedef intptr_t  iptr;
typedef uintptr_t uptr;

#define Bytes(n) ((size)n)
#define Kilobytes(n) (Bytes(n)*1024)
#define Megabytes(n) (Kilobytes(n)*1024)
#define Gigabytes(n) (Megabytes(n)*1024)

global_variable b32 Assert_Not_Implemented = 0;
global_variable b32 Assert_This_Path_Is_Invalid = 0;

#define Str_Red(string) "\033[1;31m" string "\033[0m"
#define Str_Green(string) "\033[1;32m" string "\033[0m"

#define Assert(expr) { \
    if (!(expr)) { \
        printf(Str_Red("[ASSERT FAILED(line %d)]: expr `" #expr "` (%s, %s)") "\n", __LINE__, __FILE__, __func__); \
        abort();\
    }\
    else { printf(Str_Green("[ASSERT PASSED(line %d)]: expr `" #expr "` (%s, %s)") "\n", __LINE__, __FILE__, __func__); }}

#define NotImplemented Assert(Assert_Not_Implemented)
#define InvalidPath    Assert(Assert_This_Path_Is_Invalid)

#endif // K_UTILITY_
