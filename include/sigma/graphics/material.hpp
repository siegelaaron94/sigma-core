#ifndef SIGMA_GRAPHICS_MATERIAL_HPP
#define SIGMA_GRAPHICS_MATERIAL_HPP

#include <sigma/config.hpp>

#include <sigma/graphics/technique.hpp>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/version.hpp>

namespace sigma {
namespace graphics {
    struct material : public technique_uniform_data {
        technique_identifier technique_id;
        bool transparent = false;

        template <class Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
            ar& boost::serialization::base_object<technique_uniform_data>(*this);
            ar& technique_id;
            ar& transparent;
        }
    };
}
}

BOOST_CLASS_VERSION(sigma::graphics::material, 1);

#endif // SIGMA_GRAPHICS_MATERIAL_HPP
