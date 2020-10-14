#ifndef K_UTILITY_
#define K_UTILITY_

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

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

#define Assert(expr) if(!(expr)) { printf("Assert Failed at %d in %s\n", __LINE__, __FILE__); *((int *)0) = 0; }

#endif // K_UTILITY_
