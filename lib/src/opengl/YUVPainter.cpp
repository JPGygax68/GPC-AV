#include <map>
#include <stdexcept>
#include <iostream>

extern "C" {
#include "libavutil/pixfmt.h"
}

#pragma warning(push)
#pragma warning(disable: 4251)
#include <glbinding/gl/gl.h>
//#include <glbinding/Binding.h>
#pragma warning(pop)
#include <gpc/gl/shader_program.hpp>
#include <gpc/gl/uniform.hpp>

#include <gpc/_av/opengl/YUVPainter.hpp>

using namespace std;

GPC_AV_NAMESPACE_START

namespace gl {

    using namespace ::gl;

    // PIMPL DECLARATION --------------------------------------------

    struct YUVPainter::Impl {

        void get_resources();
        void free_resources();

        void set_frame_size(const Size&);

        void upload_frame(const Frame&);

        auto fragment_shader() { return frag_sh; }

        //void initialize(int format, const Size &size);
        //void cleanup();
        // prepare_frame(const Frame &, bool load_image);
        //void set_modelview_matrix (const float *matrix);
        //void set_projection_matrix(const float *matrix);

        void set_shader_uniforms();

        void bind_textures();

        GLuint  frag_sh;
        GLuint  Y_tex, Cr_tex, Cb_tex;
        Size    frame_size;
    };

    // PUBLIC METHODS -----------------------------------------------

    YUVPainter::YUVPainter() : p(new Impl()) {}

    YUVPainter::~YUVPainter() = default;

    void YUVPainter::get_resources()
    {
        p->get_resources();
    }

    void YUVPainter::free_resources()
    {
        p->free_resources();
    }

    auto YUVPainter::frame_size() const -> Size
    {
        return p->frame_size;
    }

    void YUVPainter::set_frame_size(const Size &size)
    {
        p->set_frame_size(size);
    }

    void YUVPainter::upload_frame(const Frame &frame)
    {
        p->upload_frame(frame);
    }

    auto YUVPainter::fragment_shader() -> GLuint
    {
        return p->fragment_shader();
    }

    /* void YUVPainter::initialize(int format, const Size &size) 
    { 
        p->initialize(format, size); 
    } */

    // void YUVPainter::cleanup() { p->cleanup(); }

    // void YUVPainter::prepare_frame(const Frame &frame, bool load_image) { p->prepare_frame(frame, load_image); }

    /* void YUVPainter::set_modelview_matrix(const float * matrix)
    {
        p->set_modelview_matrix(matrix);
    }

    void YUVPainter::set_projection_matrix(const float * matrix)
    {
        p->set_projection_matrix(matrix);
    } */

    void YUVPainter::set_shader_uniforms()
    {
        p->set_shader_uniforms();
    }

    void YUVPainter::bind_textures()
    {
        p->bind_textures();
    }

    /* void YUVPainter::disable_texture_units()
    {
        p->disable_texture_units();
    } */

    // IMPLEMENTATION HELPERS --------------------------------------

    auto make_mono_texture()
    {
        GLuint texture;

        GL(GenTextures, 1, &texture);
        GL(BindTexture, GL_TEXTURE_2D, texture);
        GL(TexImage2D, GL_TEXTURE_2D, 0, (GLint)GL_LUMINANCE, 1920, 1080, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, nullptr);
        // TODO: does the following work when no size has been set ?
        GL(TexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)GL_LINEAR);
        GL(TexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)GL_LINEAR); //GL_NEAREST);

        return texture;
    }

    void set_texture_size(GLuint texture, unsigned int width, unsigned int height)
    {
        GL(BindTexture, GL_TEXTURE_2D, texture);
        GL(TexImage2D, GL_TEXTURE_2D, 0, (GLint)GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, nullptr);
    }

    // IMPLEMENTATION (PIMPL) ---------------------------------------

    static char vertex_shader_source[] = {
        #include "vertex.glsl.h"
    };

    static char fragment_source[] = {
        #include "fragment.glsl.h"
    };

    /* void YUVPainter::Impl::initialize(int format, const Size &size)
    {
        shader_program = ::gpc::gl::buildShaderProgram(vertex_shader_source, fragment_source);
#ifdef _DEBUG
        cerr << "YUV Painter shader program info log:" << endl << ::gpc::gl::getProgramInfoLog(shader_program) << endl;
#endif

        frame_size = size;

        switch (format)
        {
        case AV_PIX_FMT_YUV420P: case AV_PIX_FMT_YUVJ420P:
            Y_tex  = make_mono_texture(size.w, size.h);
            Cr_tex = make_mono_texture(size.w / 2, size.h / 2);
            Cb_tex = make_mono_texture(size.w / 2, size.h / 2);
            break;
        default:
            throw std::runtime_error("YUVPainter.initialize: unsupported");
        }
    } */

    void YUVPainter::Impl::get_resources()
    {
        frag_sh = GL(CreateShader, GL_FRAGMENT_SHADER);

        gpc::gl::compileShader(frag_sh, fragment_source);

        Y_tex  = make_mono_texture();
        Cr_tex = make_mono_texture();
        Cb_tex = make_mono_texture();
    }

    void YUVPainter::Impl::free_resources()
    {
        GL(DeleteTextures, 1, &Y_tex ); Y_tex  = 0;
        GL(DeleteTextures, 1, &Cr_tex); Cr_tex = 0;
        GL(DeleteTextures, 1, &Cb_tex); Cb_tex = 0;
        GL(DeleteShader, frag_sh); frag_sh = 0;
    }

    void YUVPainter::Impl::set_frame_size(const Size &size)
    {
        if (size.w != frame_size.w || size.h != frame_size.h)
        {
            set_texture_size(Y_tex, size.w, size.h);
            set_texture_size(Cr_tex, size.w / 2, size.h / 2);
            set_texture_size(Cb_tex, size.w / 2, size.h / 2);

            frame_size = size;
        }
    }

    void YUVPainter::Impl::upload_frame(const Frame &frame)
    {
        assert(frame_size.w != 0 && frame_size.h != 0);

        GL(ActiveTexture, GL_TEXTURE0 + 0);
        GL(BindTexture, GL_TEXTURE_2D, Y_tex);
        GL(TexSubImage2D, GL_TEXTURE_2D, 0, 0, 0, frame_size.w, frame_size.h, GL_LUMINANCE, GL_UNSIGNED_BYTE, frame.y);

        GL(ActiveTexture, GL_TEXTURE0 + 1);
        GL(BindTexture, GL_TEXTURE_2D, Cb_tex);
        GL(TexSubImage2D, GL_TEXTURE_2D, 0, 0, 0, frame_size.w / 2, frame_size.h / 2, GL_LUMINANCE, GL_UNSIGNED_BYTE, frame.u);

        GL(ActiveTexture, GL_TEXTURE0 + 2);
        GL(BindTexture, GL_TEXTURE_2D, Cr_tex);
        GL(TexSubImage2D, GL_TEXTURE_2D, 0, 0, 0, frame_size.w / 2, frame_size.h / 2, GL_LUMINANCE, GL_UNSIGNED_BYTE, frame.v);
    }

    /* void YUVPainter::Impl::prepare_frame(const Frame &frame, bool load_image)
    {
        assert(frame_size.w != 0 && frame_size.h != 0);

        GL(ActiveTexture, GL_TEXTURE0 + 0);
        GL(BindTexture, GL_TEXTURE_2D, Y_tex);
        if (load_image) GL(TexSubImage2D, GL_TEXTURE_2D, 0, 0, 0, frame_size.w, frame_size.h, GL_LUMINANCE, GL_UNSIGNED_BYTE, frame.y);

        GL(ActiveTexture, GL_TEXTURE0 + 1);
        GL(BindTexture, GL_TEXTURE_2D, Cb_tex);
        if (load_image) GL(TexSubImage2D, GL_TEXTURE_2D, 0, 0, 0, frame_size.w / 2, frame_size.h / 2, GL_LUMINANCE, GL_UNSIGNED_BYTE, frame.u);

        GL(ActiveTexture, GL_TEXTURE0 + 2);
        GL(BindTexture, GL_TEXTURE_2D, Cr_tex);
        if (load_image) GL(TexSubImage2D, GL_TEXTURE_2D, 0, 0, 0, frame_size.w / 2, frame_size.h / 2, GL_LUMINANCE, GL_UNSIGNED_BYTE, frame.v);

        GL(UseProgram, shader_program);

        ::gpc::gl::setUniform(2, 0);    // Texture unit for Y
        ::gpc::gl::setUniform(3, 1);    // Texture unit for Cr
        ::gpc::gl::setUniform(4, 2);    // Texture unit for Cb
    } */

    void YUVPainter::Impl::set_shader_uniforms()
    {
        ::gpc::gl::setUniform(2, 0);    // Texture unit for Y
        ::gpc::gl::setUniform(3, 1);    // Texture unit for Cr
        ::gpc::gl::setUniform(4, 2);    // Texture unit for Cb
    }
    
    void YUVPainter::Impl::bind_textures()
    {
        GL(ActiveTexture, GL_TEXTURE0 + 0);
        GL(BindTexture, GL_TEXTURE_2D, Y_tex);

        GL(ActiveTexture, GL_TEXTURE0 + 1);
        GL(BindTexture, GL_TEXTURE_2D, Cb_tex);

        GL(ActiveTexture, GL_TEXTURE0 + 2);
        GL(BindTexture, GL_TEXTURE_2D, Cr_tex);

        GL(ActiveTexture, GL_TEXTURE0 + 0); // for convention only
    }

    /* void YUVPainter::Impl::disable_texture_units()
    {
        for (int i = 0; i < 3; i++)
        {
            GL(ActiveTexture, GL_TEXTURE0 + i);
            GL(BindTexture, GL_TEXTURE_2D, 0);
            GL(Disable, GL_TEXTURE_2D);
        }

        GL(ActiveTexture, GL_TEXTURE0);
    } */

    /* void YUVPainter::Impl::set_modelview_matrix(const float * matrix)
    {
        ::gpc::gl::setUniformMatrix4(0, matrix);
    }

    void YUVPainter::Impl::set_projection_matrix(const float * matrix)
    {
        ::gpc::gl::setUniformMatrix4(1, matrix);
    } */

} // ns gl

GPC_AV_NAMESPACE_END
