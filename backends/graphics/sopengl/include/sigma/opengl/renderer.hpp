#ifndef SIGMA_GRAPHICS_OPENGL_CONTEXT_HPP
#define SIGMA_GRAPHICS_OPENGL_CONTEXT_HPP

#include <sigma/graphics/renderer.hpp>

#include <sigma/opengl/config.hpp>
#include <sigma/opengl/frame_buffer.hpp>
#include <sigma/opengl/geometry_buffer.hpp>
#include <sigma/opengl/material.hpp>
#include <sigma/opengl/post_process_effect.hpp>
#include <sigma/opengl/render_uniforms.hpp>
#include <sigma/opengl/shader.hpp>
#include <sigma/opengl/static_mesh.hpp>
#include <sigma/opengl/texture.hpp>
#include <sigma/opengl/uniform_buffer.hpp>

namespace sigma {

namespace opengl {
    class renderer : public graphics::renderer {
    public:
        renderer(glm::ivec2 size);

        virtual ~renderer();

        virtual graphics::texture_manager& textures() override;

        virtual graphics::shader_manager& shaders() override;

        virtual graphics::material_manager& materials() override;

        virtual graphics::static_mesh_manager& static_meshes() override;

        virtual graphics::post_process_effect_manager& effects() override;

        virtual void resize(glm::uvec2 size) override;

        void geometry_pass(const graphics::view_port& viewport, bool transparent);

        void light_pass(const graphics::view_port& viewport);

        // void point_light_outside_stencil_optimization(glm::vec3 view_space_position, float radius);

        virtual void render(const graphics::view_port& viewport) override;

    private:
        static const resource::identifier POINT_LIGHT_EFFECT;
        static const resource::identifier POINT_LIGHT_STENCIL_EFFECT;
        static const resource::identifier DIRECTIONAL_LIGHT_EFFECT;
        static const resource::identifier VIGNETTE_EFFECT;
        static const resource::identifier GAMMA_CONVERSION_EFFECT;

        renderer(const renderer&) = delete;
        renderer& operator=(const renderer&) = delete;

        default_frame_buffer default_fbo_;
        geometry_buffer gbuffer_;
        uniform_buffer<standard_uniforms> standard_uniforms_;

        opengl::texture_manager textures_;
        opengl::shader_manager shaders_;
        opengl::material_manager materials_;
        opengl::static_mesh_manager static_meshes_;
        opengl::post_process_effect_manager effects_;

        resource::handle<graphics::post_process_effect> stencil_clear_effect_;
        resource::handle<graphics::post_process_effect> texture_blit_effect_;

        // TODO were should these go?
        GLint point_light_color_location_;
        GLint point_light_position_location_;
        GLint point_light_radius_location_;
        GLint point_light_falloff_location_;
        GLint point_light_intensity_location_;
        resource::handle<graphics::post_process_effect> point_light_effect_;
        resource::handle<graphics::post_process_effect> point_light_stencil_effect_;

        // TODO were should these go?
        GLint directional_light_color_location_;
        GLint directional_light_direction_location_;
        GLint directional_light_intensity_location_;
        resource::handle<graphics::post_process_effect> directional_light_effect_;

        resource::handle<graphics::post_process_effect> vignette_effect_;

        resource::handle<graphics::post_process_effect> gamma_conversion_;
    };
}
}

#endif // SIGMA_GRAPHICS_OPENGL_CONTEXT_HPP
