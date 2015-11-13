#pragma once

#include <cstdint>
#include <memory>

#include "../config.hpp"

#include "../VideoFrame.hpp"

GPC_AV_NAMESPACE_START

namespace gl {

    class YUVPainter {
    public:
        typedef unsigned int GLuint; // We define this here so we don't have to import a GL header
        typedef VideoFrame::Size Size;
        typedef struct {
            const uint8_t *y, *u, *v;
        } Frame;

        YUVPainter();
        ~YUVPainter();

        /** Call this when the OpenGL context that will be used when painting is active
            (typically right after you've created it.
         */
        void initialize(int format, const Size &size);

        /** The OpenGL context must also be active when cleaning up.
         */
        void cleanup();

        /** This will not only upload the Y, U and V planes into their respective textures
            (allocated and initialized by the initialize() call), but also select the yuv painter
            shader program and set its texture parameters (uniforms).
            However before drawing, it is necessary to pass the modelview and projection matrices,
            as 4x4 floats at locations 0 and 1, respectively. You can use the methods 
            set_modelview_matrix() and set_projection_matrix() as helpers, but you can also
            do that yourself if you prefer.
            All that remains after that is to actually draw the rectangle (note that for each
            vertex, both vertex and texture coordinates must be provided: vertex as 4 floats,
            texture as 2 floats.
         */
        void load_frame(const Frame &);

        void set_modelview_matrix (const float *matrix);

        void set_projection_matrix(const float *matrix);

        void disable_texture_units();

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
