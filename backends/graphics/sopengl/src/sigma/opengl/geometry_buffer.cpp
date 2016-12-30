#include <sigma/opengl/geometry_buffer.hpp>

#include <sigma/opengl/util.hpp>

namespace sigma {
namespace opengl {
    geometry_buffer::geometry_buffer(glm::ivec2 size)
        : frame_buffer(size)
        , position_texture_(internal_format::RGB16F, size)
        , diffuse_texture_(internal_format::RGB8, size)
        , normal_texture_(internal_format::RGB16F, size)
        , depth_stencil_texture_(internal_format::DEPTH32F_STENCIL8, size)
        , images_{ texture{ internal_format::RGB16F, size }, texture{ internal_format::RGB16F, size } }
        , input_image_(0)
        , output_image_(1)
    {
        attach(WORLD_POSITION_ATTACHMENT, position_texture_);
        attach(DIFFUSE_COLOR_ATTACHMENT, diffuse_texture_);
        attach(NORMAL_ATTACHMENT, normal_texture_);
        attach(OUTPUT_IMAGE_ATTACHMENT, images_[output_image_]);
        attach(frame_buffer::attachment::DEPTH_STENCIL, depth_stencil_texture_);
        draw_buffers({ WORLD_POSITION_ATTACHMENT, DIFFUSE_COLOR_ATTACHMENT, NORMAL_ATTACHMENT });
    }

    void geometry_buffer::bind_for_geometry_pass()
    {
        draw_buffers({ WORLD_POSITION_ATTACHMENT, DIFFUSE_COLOR_ATTACHMENT, NORMAL_ATTACHMENT });
    }

    void geometry_buffer::bind_for_stencil_pass()
    {
        draw_buffers({ attachment::NONE });
    }

    void geometry_buffer::bind_for_effect_pass()
    {
        draw_buffers({ OUTPUT_IMAGE_ATTACHMENT });

        GL_CHECK(glActiveTexture(GLenum(WORLD_POSITION_TEXTURE_UINT)));
        position_texture_.bind();

        GL_CHECK(glActiveTexture(GLenum(DIFFUSE_COLOR_TEXTURE_UINT)));
        diffuse_texture_.bind();

        GL_CHECK(glActiveTexture(GLenum(NORMAL_TEXTURE_UINT)));
        normal_texture_.bind();

        GL_CHECK(glActiveTexture(GLenum(NORMAL_TEXTURE_UINT)));
        normal_texture_.bind();

        GL_CHECK(glActiveTexture(GLenum(INPUT_IMAGE_TEXTURE_UINT)));
        images_[input_image_].bind();
    }

    void geometry_buffer::clear_input_image(glm::vec4 color)
    {
        draw_buffers({ OUTPUT_IMAGE_ATTACHMENT });
        GL_CHECK(glClearColor(color.r, color.g, color.b, color.a));
        GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));
    }

    void geometry_buffer::swap_input_image()
    {
        output_image_++;
        output_image_ %= 2;
        input_image_++;
        input_image_ %= 2;
        attach(OUTPUT_IMAGE_ATTACHMENT, images_[output_image_]);
    }
}
}