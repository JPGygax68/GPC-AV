#include <gpc/_av/opengl/YUVPainter.hpp>

GPC_AV_NAMESPACE_START

namespace gl {

    // PIMPL DECLARATION --------------------------------------------

    struct YUVPainter::Impl {
        void initialize();
        void cleanup();
        void load_frame(const Frame &, const Size &);
    };

    // PUBLIC METHODS -----------------------------------------------

    YUVPainter::YUVPainter() : p(new Impl()) {}

    YUVPainter::~YUVPainter() = default;

    void YUVPainter::initialize() { p->initialize(); }

    void YUVPainter::cleanup() { p->cleanup(); }

    void YUVPainter::load_frame(const Frame &frame, const Size &size) { p->load_frame(frame, size); }

    // IMPLEMENTATION (PIMPL) ---------------------------------------

    void YUVPainter::Impl::initialize()
    {
        // TODO:
        // - compile and link shader program
        // - allocate and configure textures (Y, U, V)
    }

    void YUVPainter::Impl::cleanup()
    {
        // TODO:
        // - deallocate shader program
        // - deallocate Y, U and V textures
    }

    void YUVPainter::Impl::load_frame(const Frame &frame, const Size &size) 
    { 
        // TODO: us glTexSubImage2D() to upload the three textures
    }

} // ns gl

GPC_AV_NAMESPACE_END
