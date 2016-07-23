#ifndef SIGMAFIVE_EDITOR_WIDGETS_OPENGLWIDGET_HPP
#define SIGMAFIVE_EDITOR_WIDGETS_OPENGLWIDGET_HPP

#include <sigmafive/opengl/context.hpp>

#include <QOpenGLWidget>

#include <sigmafive/editor/trackball_controller.hpp>

namespace sigmafive {
namespace editor {
    class opengl_widget : public QOpenGLWidget {
        Q_OBJECT
    public:
        opengl_widget(QWidget* parent = nullptr);

        ~opengl_widget();

        void set_viewport(graphics::view_port* viewport);

        graphics::texture_cache& textures();

        graphics::shader_cache& shaders();

        graphics::material_cache& materials();

        graphics::static_mesh_cache& static_meshes();

        void initializeGL() override;

        void resizeGL(int w, int h) override;

        void paintGL() override;

    signals:
        void initialized(opengl_widget* widget);

    private:
        graphics::view_port* viewport_;
        std::unique_ptr<opengl::context> context_;
    };
}
}

#endif // SIGMAFIVE_EDITOR_WIDGETS_OPENGLWIDGET_HPP
