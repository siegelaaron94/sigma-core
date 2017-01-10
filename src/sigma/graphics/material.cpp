#include <sigma/graphics/material.hpp>

namespace sigma {
namespace graphics {
    material::material(const material_data& data)
    {
        transparent_ = data.transparent;
        for (const auto& shdr : data.shaders)
            shaders_[shdr.first] = resource::handle<graphics::shader>{ shdr.second };
        for (const auto& txt : data.textures)
            textures_.push_back(std::make_pair(txt.first, resource::handle<graphics::texture>{ txt.second }));
        // TODO cubemap
        // for (const auto& cube : data.cubemaps)
        //     textures_.push_back(std::make_pair(cube.first, resource::handle<cubemap>{ cube.second }));
    }

    bool material::is_transparent() const
    {
        return transparent_;
    }

    bool material::has_shader(graphics::shader_type type) const
    {
        return shaders_.count(type) == 1;
    }

    resource::handle<graphics::shader>& material::shader(graphics::shader_type type)
    {
        return shaders_[type];
    }

    void material::set_shader(graphics::shader_type type, resource::handle<graphics::shader> shdr)
    {
        // TODO maybe if the shader handle passed in is not valid
        // and one of the other shaders have is valid
        // copy the manager into the invalid handle.
        if (shaders_[type] != shdr)
            shaders_[type] = shdr;
    }

    std::size_t material::texture_count() const
    {
        return textures_.size();
    }

    resource::handle<graphics::texture>& material::texture(std::size_t index)
    {
        return textures_[index].second;
    }

    void material::set_texture(std::size_t index, resource::handle<graphics::texture> txt)
    {
        // TODO maybe if the texture handle passed in is not valid
        // and one of the other textures have is valid
        // copy the manager into the invalid handle.
        if (textures_[index].second != txt)
            textures_[index].second = txt;
    }
}
}
