#pragma once

#include <cstdint>
#include <memory>

#include "../config.hpp"

GPC_AV_NAMESPACE_START

namespace gl {

    class YUVPainter {
    public:
        typedef unsigned int GLuint; // We define this here so we don't have to import a GL header
        typedef struct {
            unsigned int w, h;
        } Size;
        typedef struct {
            const uint8_t *y, *u, *v;
        } Frame;

        YUVPainter();
        ~YUVPainter();

        /** Call this when the OpenGL context that will be used when painting is active
            (typically right after you've created it.
         */
        void initialize();

        /** The OpenGL context must also be active when cleaning up.
         */
        void cleanup();

        /** This will not only upload the Y, U and V planes into their respective textures
            allocated and initialized by the initialize() call, but also select the yuv painter
            shader program and set its parameters (uniforms).
            All that remains to be done after load_frame() is actually drawing the rectangle
            (note that both vertex and texture coordinates must be provided).
         */
        void load_frame(const Frame &, const Size &);

        /** The compiled and linked shader program to use for rendering. Use as argument to
        glUseProgram().
        */
        //auto program()->GLuint;

        /*
        auto y_texture() -> GLuint;
        auto u_texture() -> GLuint;
        auto v_texture() -> GLuint;
        */

    private:
        struct Impl;

        std::unique_ptr<Impl>   p;
    };
}

GPC_AV_NAMESPACE_END
