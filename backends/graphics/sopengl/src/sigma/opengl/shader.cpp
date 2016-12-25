#include <sigma/opengl/shader.hpp>

#include <sigma/opengl/util.hpp>

#include <cassert>
#include <iostream>
#include <vector>

namespace sigma {
namespace opengl {
    shader::shader(shader_type type, std::string source)
    {
        const char* src = source.c_str();

        GL_CHECK(object_ = glCreateShader(GLenum(type)));
        GL_CHECK(glShaderSource(object_, 1, &src, nullptr));
        GL_CHECK(glCompileShader(object_));

        GLint compiled;
        glGetShaderiv(object_, GL_COMPILE_STATUS, &compiled);
        if (compiled == GL_FALSE) {
            GLint length = 0;
            glGetShaderiv(object_, GL_INFO_LOG_LENGTH, &length);

            // The length includes the NULL character
            std::vector<GLchar> error_buffer(length);
            glGetShaderInfoLog(object_, length, &length, error_buffer.data());

            std::string error_string(error_buffer.begin(), error_buffer.end());
            std::cout << error_string << std::endl;

            std::abort();
        }
    }

    shader::~shader()
    {
        glDeleteShader(object_);
    }

    GLuint shader::get_object() const
    {
        return object_;
    }
}
}
