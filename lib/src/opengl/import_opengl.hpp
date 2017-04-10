#pragma once

#ifdef USE_GLBINDING

#pragma warning(push)
#pragma warning(disable: 4251)
#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>
using namespace gl;
#pragma warning(pop)

#else

#ifdef _WIN32
#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#endif
#include <GL/glew.h>

#endif
