
// ==================
// Standard stuff.
// (stdint not included.)
// ==================
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>      // TODO: Some day i'll blow this line off out of existence.

#if _MSC_VER
#include <windows.h>
#endif

// ==================
// Glew / Glfw Stuff.
// ==================
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// ==================
// Stb Image stuff.
// ==================
#define STB_IMAGE_IMPLEMENTATION
#include "deps/stb_image.h" // TODO: Some day i'll blow this line off out of existence.

