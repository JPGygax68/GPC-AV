#pragma once

#ifdef USE_GLBINDING

#pragma warning(push)
#pragma warning(disable: 4251)
#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>
using namespace gl;
#pragma warning(pop)

#else

#include <GL/glew.h>

#endif
