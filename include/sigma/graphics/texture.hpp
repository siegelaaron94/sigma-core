#ifndef SIGMA_GRAPHICS_TEXTURE_HPP
#define SIGMA_GRAPHICS_TEXTURE_HPP

#include <sigma/config.hpp>
#include <sigma/reflect.hpp>
#include <sigma/resource/resource.hpp>
#include <sigma/util/glm_serialize.hpp>

#include <glm/vec2.hpp>

#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/serialization/vector.hpp>

#include <vector>

namespace sigma {
namespace graphics {

    enum class R_EXPORT() texture_filter {
        LINEAR,
        NEAREST,
        NONE
    };

    enum class R_EXPORT() texture_format {
        RGB8,
        RGBA8,
        RGB32F
    };

    class texture {
        glm::ivec2 size_;
        texture_format format_;
        texture_filter minification_filter_;
        texture_filter magnification_filter_;
        texture_filter mipmap_filter_;
        std::vector<char> data_;

    public:
        texture();

        texture(const boost::gil::rgb8c_view_t& view,
            texture_filter minification_filter = texture_filter::LINEAR,
            texture_filter magnification_filter = texture_filter::LINEAR,
            texture_filter mipmap_filter = texture_filter::LINEAR);

        texture(const boost::gil::rgba8c_view_t& view,
            texture_filter minification_filter = texture_filter::LINEAR,
            texture_filter magnification_filter = texture_filter::LINEAR,
            texture_filter mipmap_filter = texture_filter::LINEAR);

        texture(const boost::gil::rgb32fc_view_t& view,
            texture_filter minification_filter = texture_filter::LINEAR,
            texture_filter magnification_filter = texture_filter::LINEAR,
            texture_filter mipmap_filter = texture_filter::LINEAR);

        const glm::ivec2& size() const noexcept;

        texture_format format() const noexcept;

        texture_filter minification_filter() const noexcept;

        texture_filter magnification_filter() const noexcept;

        texture_filter mipmap_filter() const noexcept;

        const char* data() const;

        template <class Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
            ar& size_;
            ar& format_;
            ar& minification_filter_;
            ar& magnification_filter_;
            ar& mipmap_filter_;
            ar& data_;
        }
    };
}
}

REGISTER_RESOURCE(sigma::graphics::texture, texture, 1)

#endif // SIGMA_GRAPHICS_TEXTURE_HPP
