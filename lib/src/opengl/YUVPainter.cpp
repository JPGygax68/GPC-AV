#include <map>
#include <stdexcept>
#include <iostream>

extern "C" {
#include "libavutil/pixfmt.h"
}

#include <gpc/gl/shader_program.hpp>
#include <gpc/gl/uniform.hpp>

#include <gpc/_av/opengl/YUVPainter.hpp>

using namespace std;

GPC_AV_NAMESPACE_START

namespace gl {

    // PIMPL DECLARATION --------------------------------------------

    struct YUVPainter::Impl {
        void initialize(int format, const Size &size);
        void cleanup();
        void load_frame(const Frame &);
        void set_modelview_matrix (const float *matrix);
        void set_projection_matrix(const float *matrix);
        void disable_texture_units();

        GLuint  shader_program;
        GLuint  Y_tex, Cr_tex, Cb_tex;
        Size    frame_size;
    };

    // PUBLIC METHODS -----------------------------------------------

    YUVPainter::YUVPainter() : p(new Impl()) {}

    YUVPainter::~YUVPainter() = default;

    void YUVPainter::initialize(int format, const Size &size) 
    { 
        p->initialize(format, size); 
    }

    void YUVPainter::cleanup() { p->cleanup(); }

    void YUVPainter::load_frame(const Frame &frame) { p->load_frame(frame); }

    void YUVPainter::set_modelview_matrix(const float * matrix)
    {
        p->set_modelview_matrix(matrix);
    }

    void YUVPainter::set_projection_matrix(const float * matrix)
    {
        p->set_projection_matrix(matrix);
    }

    void YUVPainter::disable_texture_units()
    {
        p->disable_texture_units();
    }

    // IMPLEMENTATION HELPERS --------------------------------------

    auto makeMonoTexture(unsigned int width, unsigned int height)
    {
        GLuint texture;

        EXEC_GL(glGenTextures, 1, &texture);
        EXEC_GL(glBindTexture, GL_TEXTURE_2D, texture);
        EXEC_GL(glTexImage2D, GL_TEXTURE_2D, 0, (GLint) GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, nullptr);
        EXEC_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint) GL_NEAREST);
        EXEC_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint) GL_NEAREST);

        return texture;
    }

    // IMPLEMENTATION (PIMPL) ---------------------------------------

    static char vertex_shader_source[] = {
        #include "vertex.glsl.h"
    };

    static char fragment_shader_source[] = {
        #include "fragment.glsl.h"
    };

    void YUVPainter::Impl::initialize(int format, const Size &size)
    {
        shader_program = ::gpc::gl::buildShaderProgram(vertex_shader_source, fragment_shader_source);
#ifdef _DEBUG
        cerr << "YUV Painter shader program info log:" << endl << ::gpc::gl::getProgramInfoLog(shader_program) << endl;
#endif

        frame_size = size;

        switch (format)
        {
        case AV_PIX_FMT_YUV420P: case AV_PIX_FMT_YUVJ420P:
            Y_tex  = makeMonoTexture(size.w, size.h);
            Cr_tex = makeMonoTexture(size.w / 2, size.h / 2);
            Cb_tex = makeMonoTexture(size.w / 2, size.h / 2);
            break;
        default:
            throw std::runtime_error("YUVPainter.initialize: unsupported");
        }
    }

    void YUVPainter::Impl::cleanup()
    {
        EXEC_GL(glDeleteTextures, 1, &Y_tex ); Y_tex  = 0;
        EXEC_GL(glDeleteTextures, 1, &Cr_tex); Cr_tex = 0;
        EXEC_GL(glDeleteTextures, 1, &Cb_tex); Cb_tex = 0;
        EXEC_GL(glDeleteProgram, shader_program); shader_program = 0;
    }

    void YUVPainter::Impl::load_frame(const Frame &frame) 
    {
        const uint8_t *p;

        EXEC_GL(glActiveTexture, GL_TEXTURE0 + 0);
        EXEC_GL(glBindTexture, GL_TEXTURE_2D, Y_tex);
        EXEC_GL(glTexSubImage2D, GL_TEXTURE_2D, 0, 0, 0, frame_size.w   , frame_size.h     , GL_LUMINANCE, GL_UNSIGNED_BYTE, frame.y);

        EXEC_GL(glActiveTexture, GL_TEXTURE0 + 1);
        EXEC_GL(glBindTexture, GL_TEXTURE_2D, Cb_tex);
        EXEC_GL(glTexSubImage2D, GL_TEXTURE_2D, 0, 0, 0, frame_size.w / 2, frame_size.h / 2, GL_LUMINANCE, GL_UNSIGNED_BYTE, frame.u);

        EXEC_GL(glActiveTexture, GL_TEXTURE0 + 2);
        EXEC_GL(glBindTexture, GL_TEXTURE_2D, Cr_tex);
        EXEC_GL(glTexSubImage2D, GL_TEXTURE_2D, 0, 0, 0, frame_size.w / 2, frame_size.h / 2, GL_LUMINANCE, GL_UNSIGNED_BYTE, frame.v);

        EXEC_GL(glUseProgram, shader_program);

        ::gpc::gl::setUniform(2, 0);    // Texture unit for Y
        ::gpc::gl::setUniform(3, 1);    // Texture unit for Cr
        ::gpc::gl::setUniform(4, 2);    // Texture unit for Cb
    }

    void YUVPainter::Impl::disable_texture_units()
    {
        for (int i = 0; i < 3; i++)
        {
            EXEC_GL(glActiveTexture, GL_TEXTURE0 + i);
            EXEC_GL(glDisable, GL_TEXTURE_2D);
        }
    }

    void YUVPainter::Impl::set_modelview_matrix(const float * matrix)
    {
        ::gpc::gl::setUniformMatrix4(0, matrix);
    }

    void YUVPainter::Impl::set_projection_matrix(const float * matrix)
    {
        ::gpc::gl::setUniformMatrix4(1, matrix);
    }

} // ns gl

GPC_AV_NAMESPACE_END
