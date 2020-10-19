
// ==================
// Standard stuff.
// (stdint not included, it's in general.h.)
// ==================
#include <stdlib.h>
#include <stdio.h>   //NOTE(fuzzy): Used in general.h, and basically everywhere. for printf.
#include <math.h>    //NOTE(fuzzy): Used in mafs.h, cos/sin. might replace it later.

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
#include "deps/stb_image.h" // TODO(fuzzy): useful but I should be able to implement on my own.

