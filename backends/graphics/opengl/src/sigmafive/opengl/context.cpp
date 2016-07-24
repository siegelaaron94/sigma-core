#include <sigmafive/opengl/context.hpp>

#include <boost/filesystem.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <sigmafive/opengl/gl_core_4_0.h>

namespace sigmafive {
namespace opengl {
    const resource::identifier context::PLANE_STATIC_MESH{ "static_mesh://plane:Plane" };
    const resource::identifier context::FULLSCREEN_MATERIAL1{ "material://fullscreen_quad" };
    const resource::identifier context::FULLSCREEN_MATERIAL2{ "material://fullscreen_quad2" };

    context::context()
        : materials_(shaders_, textures_)
        , static_meshes_(materials_)
    {
        GL_CHECK(glClearColor(0.8, 0.8, 0.8, 1.0));
        GL_CHECK(glEnable(GL_DEPTH_TEST));
        GL_CHECK(glEnable(GL_BLEND));
        GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        GL_CHECK(glCullFace(GL_BACK));
        GL_CHECK(glEnable(GL_CULL_FACE));
        materials_.increment_reference(FULLSCREEN_MATERIAL1);
        materials_.increment_reference(FULLSCREEN_MATERIAL2);
        static_meshes_.increment_reference(PLANE_STATIC_MESH);
    }

    context::~context()
    {
        static_meshes_.decrement_reference(PLANE_STATIC_MESH);
        materials_.decrement_reference(FULLSCREEN_MATERIAL2);
        materials_.decrement_reference(FULLSCREEN_MATERIAL1);
    }

    graphics::texture_cache& context::textures()
    {
        return textures_;
    }

    graphics::shader_cache& context::shaders()
    {
        return shaders_;
    }

    graphics::material_cache& context::materials()
    {
        return materials_;
    }

    static_mesh_cache& context::static_meshes()
    {
        return static_meshes_;
    }

    void context::resize(glm::uvec2 size)
    {
        // TODO resize
    }

    void context::render(const graphics::view_port& viewport)
    {
        GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        textures_.update();
        shaders_.update();
        materials_.update();
        static_meshes_.update();

        for (auto e : viewport.entities) { // TODO use a filter here
            if (viewport.static_mesh_instances.has(e) && viewport.transforms.has(e)) {
                const auto& txform = viewport.transforms.get(e);
                const auto& mesh_inst = viewport.static_mesh_instances.get(e);
                glm::mat4 model_matrix(1.0f);
                model_matrix = glm::mat4_cast(txform.rotation) * glm::translate(glm::scale(model_matrix, txform.scale), txform.position);
                static_meshes_.render(mesh_inst, viewport.projection_matrix, viewport.view_matrix, model_matrix);
            }
        }

        // static_meshes_.render(PLANE_STATIC_MESH, FULLSCREEN_MATERIAL1,
        // viewport.projection_matrix, viewport.view_matrix, glm::mat4(1.0f));
    }
}
}
