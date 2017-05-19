#if defined(__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#pragma comment(lib, "glfw3dll.lib")
#pragma comment(lib, "opengl32.lib")
#endif

