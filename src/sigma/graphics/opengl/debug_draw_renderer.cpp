// ================================================================================================
// -*- C++ -*-
// File: sample_gl_core.cpp
// Author: Guilherme R. Lampert
// Created on: 10/12/15
// Brief: Debug Draw usage sample with Core Profile OpenGL 3+
//
// This software is in the public domain. Where that dedication is not recognized,
// you are granted a perpetual, irrevocable license to copy, distribute, and modify
// this file as you see fit.
// ================================================================================================
#include <sigma/graphics/opengl/debug_draw_renderer.hpp>

#define DEBUG_DRAW_IMPLEMENTATION
#include "debug_draw.hpp"

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>

// ========================================================
// Minimal shaders we need for the debug primitives:
// ========================================================

static const char* linePointVertShaderSrc = "\n"
                                            "#version 150\n"
                                            "\n"
                                            "in vec3 in_Position;\n"
                                            "in vec4 in_ColorPointSize;\n"
                                            "\n"
                                            "out vec4 v_Color;\n"
                                            "uniform mat4 u_MvpMatrix;\n"
                                            "\n"
                                            "void main()\n"
                                            "{\n"
                                            "    gl_Position  = u_MvpMatrix * vec4(in_Position, 1.0);\n"
                                            "    gl_PointSize = in_ColorPointSize.w;\n"
                                            "    v_Color      = vec4(in_ColorPointSize.xyz, 1.0);\n"
                                            "}\n";

// ------------------------------------------------------------------

static const char* linePointFragShaderSrc = "\n"
                                            "#version 150\n"
                                            "\n"
                                            "in vec4 v_Color;\n"
                                            "out vec4 out_FragColor;\n"
                                            "\n"
                                            "void main()\n"
                                            "{\n"
                                            "    out_FragColor = v_Color;\n"
                                            "}\n";

// ------------------------------------------------------------------

static const char* textVertShaderSrc = "\n"
                                       "#version 150\n"
                                       "\n"
                                       "in vec2 in_Position;\n"
                                       "in vec2 in_TexCoords;\n"
                                       "in vec3 in_Color;\n"
                                       "\n"
                                       "uniform vec2 u_screenDimensions;\n"
                                       "\n"
                                       "out vec2 v_TexCoords;\n"
                                       "out vec4 v_Color;\n"
                                       "\n"
                                       "void main()\n"
                                       "{\n"
                                       "    // Map to normalized clip coordinates:\n"
                                       "    float x = ((2.0 * (in_Position.x - 0.5)) / u_screenDimensions.x) - 1.0;\n"
                                       "    float y = 1.0 - ((2.0 * (in_Position.y - 0.5)) / u_screenDimensions.y);\n"
                                       "\n"
                                       "    gl_Position = vec4(x, y, 0.0, 1.0);\n"
                                       "    v_TexCoords = in_TexCoords;\n"
                                       "    v_Color     = vec4(in_Color, 1.0);\n"
                                       "}\n";

// ------------------------------------------------------------------

static const char* textFragShaderSrc = "\n"
                                       "#version 150\n"
                                       "\n"
                                       "in vec2 v_TexCoords;\n"
                                       "in vec4 v_Color;\n"
                                       "\n"
                                       "uniform sampler2D u_glyphTexture;\n"
                                       "out vec4 out_FragColor;\n"
                                       "\n"
                                       "void main()\n"
                                       "{\n"
                                       "    out_FragColor = v_Color;\n"
                                       "    out_FragColor.a = texture(u_glyphTexture, v_TexCoords).r;\n"
                                       "}\n";

// GL error enum to printable string.
static inline const char* errorToString(const GLenum errorCode)
{
    switch (errorCode) {
    case GL_NO_ERROR:
        return "GL_NO_ERROR";
    case GL_INVALID_ENUM:
        return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE:
        return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION:
        return "GL_INVALID_OPERATION";
    case GL_OUT_OF_MEMORY:
        return "GL_OUT_OF_MEMORY";
    default:
        return "Unknown GL error";
    }
}

namespace sigma {
namespace opengl {
    debug_draw_renderer::debug_draw_renderer()
        : mvpMatrix(glm::mat4{})
        , linePointProgram(0)
        , linePointProgram_MvpMatrixLocation(-1)
        , textProgram(0)
        , textProgram_GlyphTextureLocation(-1)
        , textProgram_ScreenDimensions(-1)
        , linePointVAO(0)
        , linePointVBO(0)
        , textVAO(0)
        , textVBO(0)
    {
        // Default OpenGL states:
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

        // This has to be enabled since the point drawing shader will use gl_PointSize.
        glEnable(GL_PROGRAM_POINT_SIZE);

        setupShaderPrograms();
        setupVertexBuffers();
    }

    debug_draw_renderer::~debug_draw_renderer()
    {
        glDeleteProgram(linePointProgram);
        glDeleteProgram(textProgram);

        glDeleteVertexArrays(1, &linePointVAO);
        glDeleteBuffers(1, &linePointVBO);

        glDeleteVertexArrays(1, &textVAO);
        glDeleteBuffers(1, &textVBO);
    }

    void debug_draw_renderer::drawPointList(const dd::DrawVertex* points, int count, bool depthEnabled)
    {
        assert(points != nullptr);
        assert(count > 0 && count <= DEBUG_DRAW_VERTEX_BUFFER_SIZE);

        glBindVertexArray(linePointVAO);
        glUseProgram(linePointProgram);

        glUniformMatrix4fv(linePointProgram_MvpMatrixLocation,
            1, GL_FALSE, glm::value_ptr(mvpMatrix));

        if (depthEnabled) {
            glEnable(GL_DEPTH_TEST);
        } else {
            glDisable(GL_DEPTH_TEST);
        }

        // NOTE: Could also use glBufferData to take advantage of the buffer orphaning trick...
        glBindBuffer(GL_ARRAY_BUFFER, linePointVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(dd::DrawVertex), points);

        // Issue the draw call:
        glDrawArrays(GL_POINTS, 0, count);

        glUseProgram(0);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        checkGLError(__FILE__, __LINE__);
    }

    void debug_draw_renderer::drawLineList(const dd::DrawVertex* lines, int count, bool depthEnabled)
    {
        assert(lines != nullptr);
        assert(count > 0 && count <= DEBUG_DRAW_VERTEX_BUFFER_SIZE);

        glBindVertexArray(linePointVAO);
        glUseProgram(linePointProgram);

        glUniformMatrix4fv(linePointProgram_MvpMatrixLocation,
            1, GL_FALSE, glm::value_ptr(mvpMatrix));

        if (depthEnabled) {
            glEnable(GL_DEPTH_TEST);
        } else {
            glDisable(GL_DEPTH_TEST);
        }

        // NOTE: Could also use glBufferData to take advantage of the buffer orphaning trick...
        glBindBuffer(GL_ARRAY_BUFFER, linePointVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(dd::DrawVertex), lines);

        // Issue the draw call:
        glDrawArrays(GL_LINES, 0, count);

        glUseProgram(0);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        checkGLError(__FILE__, __LINE__);
    }

    void debug_draw_renderer::drawGlyphList(const dd::DrawVertex* glyphs, int count, dd::GlyphTextureHandle glyphTex)
    {
        assert(glyphs != nullptr);
        assert(count > 0 && count <= DEBUG_DRAW_VERTEX_BUFFER_SIZE);

        glBindVertexArray(textVAO);
        glUseProgram(textProgram);

        // These doesn't have to be reset every draw call, I'm just being lazy ;)
        glUniform1i(textProgram_GlyphTextureLocation, 0);
        glUniform2f(textProgram_ScreenDimensions,
            static_cast<GLfloat>(windowWidth),
            static_cast<GLfloat>(windowHeight));

        if (glyphTex != nullptr) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, handleToGL(glyphTex));
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);

        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(dd::DrawVertex), glyphs);

        glDrawArrays(GL_TRIANGLES, 0, count); // Issue the draw call

        glDisable(GL_BLEND);
        glUseProgram(0);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        checkGLError(__FILE__, __LINE__);
    }

    dd::GlyphTextureHandle debug_draw_renderer::createGlyphTexture(int width, int height, const void* pixels)
    {
        assert(width > 0 && height > 0);
        assert(pixels != nullptr);

        GLuint textureId = 0;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);
        checkGLError(__FILE__, __LINE__);

        return GLToHandle(textureId);
    }

    void debug_draw_renderer::destroyGlyphTexture(dd::GlyphTextureHandle glyphTex)
    {
        if (glyphTex == nullptr) {
            return;
        }

        const GLuint textureId = handleToGL(glyphTex);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &textureId);
    }

    void debug_draw_renderer::setupShaderPrograms()
    {
        //
        // Line/point drawing shader:
        //
        {
            GLuint linePointVS = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(linePointVS, 1, &linePointVertShaderSrc, nullptr);
            compileShader(linePointVS);

            GLint linePointFS = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(linePointFS, 1, &linePointFragShaderSrc, nullptr);
            compileShader(linePointFS);

            linePointProgram = glCreateProgram();
            glAttachShader(linePointProgram, linePointVS);
            glAttachShader(linePointProgram, linePointFS);

            glBindAttribLocation(linePointProgram, 0, "in_Position");
            glBindAttribLocation(linePointProgram, 1, "in_ColorPointSize");
            linkProgram(linePointProgram);

            linePointProgram_MvpMatrixLocation = glGetUniformLocation(linePointProgram, "u_MvpMatrix");
            if (linePointProgram_MvpMatrixLocation < 0) {
                std::cerr << "Unable to get u_MvpMatrix uniform location!" << std::endl;
            }
            checkGLError(__FILE__, __LINE__);
        }

        //
        // Text rendering shader:
        //
        {
            GLuint textVS = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(textVS, 1, &textVertShaderSrc, nullptr);
            compileShader(textVS);

            GLint textFS = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(textFS, 1, &textFragShaderSrc, nullptr);
            compileShader(textFS);

            textProgram = glCreateProgram();
            glAttachShader(textProgram, textVS);
            glAttachShader(textProgram, textFS);

            glBindAttribLocation(textProgram, 0, "in_Position");
            glBindAttribLocation(textProgram, 1, "in_TexCoords");
            glBindAttribLocation(textProgram, 2, "in_Color");
            linkProgram(textProgram);

            textProgram_GlyphTextureLocation = glGetUniformLocation(textProgram, "u_glyphTexture");
            if (textProgram_GlyphTextureLocation < 0) {
                std::cerr << "Unable to get u_glyphTexture uniform location!" << std::endl;
            }

            textProgram_ScreenDimensions = glGetUniformLocation(textProgram, "u_screenDimensions");
            if (textProgram_ScreenDimensions < 0) {
                std::cerr << "Unable to get u_screenDimensions uniform location!" << std::endl;
            }

            checkGLError(__FILE__, __LINE__);
        }
    }

    void debug_draw_renderer::setupVertexBuffers()
    {
        //
        // Lines/points vertex buffer:
        //
        {
            glGenVertexArrays(1, &linePointVAO);
            glGenBuffers(1, &linePointVBO);
            checkGLError(__FILE__, __LINE__);

            glBindVertexArray(linePointVAO);
            glBindBuffer(GL_ARRAY_BUFFER, linePointVBO);

            // RenderInterface will never be called with a batch larger than
            // DEBUG_DRAW_VERTEX_BUFFER_SIZE vertexes, so we can allocate the same amount here.
            glBufferData(GL_ARRAY_BUFFER, DEBUG_DRAW_VERTEX_BUFFER_SIZE * sizeof(dd::DrawVertex), nullptr, GL_STREAM_DRAW);
            checkGLError(__FILE__, __LINE__);

            // Set the vertex format expected by 3D points and lines:
            std::size_t offset = 0;

            glEnableVertexAttribArray(0); // in_Position (vec3)
            glVertexAttribPointer(
                /* index     = */ 0,
                /* size      = */ 3,
                /* type      = */ GL_FLOAT,
                /* normalize = */ GL_FALSE,
                /* stride    = */ sizeof(dd::DrawVertex),
                /* offset    = */ reinterpret_cast<void*>(offset));
            offset += sizeof(float) * 3;

            glEnableVertexAttribArray(1); // in_ColorPointSize (vec4)
            glVertexAttribPointer(
                /* index     = */ 1,
                /* size      = */ 4,
                /* type      = */ GL_FLOAT,
                /* normalize = */ GL_FALSE,
                /* stride    = */ sizeof(dd::DrawVertex),
                /* offset    = */ reinterpret_cast<void*>(offset));

            checkGLError(__FILE__, __LINE__);

            // VAOs can be a pain in the neck if left enabled...
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        //
        // Text rendering vertex buffer:
        //
        {
            glGenVertexArrays(1, &textVAO);
            glGenBuffers(1, &textVBO);
            checkGLError(__FILE__, __LINE__);

            glBindVertexArray(textVAO);
            glBindBuffer(GL_ARRAY_BUFFER, textVBO);

            // NOTE: A more optimized implementation might consider combining
            // both the lines/points and text buffers to save some memory!
            glBufferData(GL_ARRAY_BUFFER, DEBUG_DRAW_VERTEX_BUFFER_SIZE * sizeof(dd::DrawVertex), nullptr, GL_STREAM_DRAW);
            checkGLError(__FILE__, __LINE__);

            // Set the vertex format expected by the 2D text:
            std::size_t offset = 0;

            glEnableVertexAttribArray(0); // in_Position (vec2)
            glVertexAttribPointer(
                /* index     = */ 0,
                /* size      = */ 2,
                /* type      = */ GL_FLOAT,
                /* normalize = */ GL_FALSE,
                /* stride    = */ sizeof(dd::DrawVertex),
                /* offset    = */ reinterpret_cast<void*>(offset));
            offset += sizeof(float) * 2;

            glEnableVertexAttribArray(1); // in_TexCoords (vec2)
            glVertexAttribPointer(
                /* index     = */ 1,
                /* size      = */ 2,
                /* type      = */ GL_FLOAT,
                /* normalize = */ GL_FALSE,
                /* stride    = */ sizeof(dd::DrawVertex),
                /* offset    = */ reinterpret_cast<void*>(offset));
            offset += sizeof(float) * 2;

            glEnableVertexAttribArray(2); // in_Color (vec4)
            glVertexAttribPointer(
                /* index     = */ 2,
                /* size      = */ 4,
                /* type      = */ GL_FLOAT,
                /* normalize = */ GL_FALSE,
                /* stride    = */ sizeof(dd::DrawVertex),
                /* offset    = */ reinterpret_cast<void*>(offset));

            checkGLError(__FILE__, __LINE__);

            // Ditto.
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }

    GLuint debug_draw_renderer::handleToGL(dd::GlyphTextureHandle handle)
    {
        const std::size_t temp = reinterpret_cast<std::size_t>(handle);
        return static_cast<GLuint>(temp);
    }

    dd::GlyphTextureHandle debug_draw_renderer::GLToHandle(const GLuint id)
    {
        const std::size_t temp = static_cast<std::size_t>(id);
        return reinterpret_cast<dd::GlyphTextureHandle>(temp);
    }

    void debug_draw_renderer::checkGLError(const char* file, const int line)
    {
        GLenum err = 0;
        char msg[1024];
        while ((err = glGetError()) != 0) {
            std::snprintf(msg, sizeof(msg), "%s(%d) : GL_CORE_ERROR=0x%X - %s", file, line, err, errorToString(err));
            std::cerr << msg << std::endl;
        }
    }

    void debug_draw_renderer::compileShader(const GLuint shader)
    {
        glCompileShader(shader);
        checkGLError(__FILE__, __LINE__);

        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        checkGLError(__FILE__, __LINE__);

        if (status == GL_FALSE) {
            GLchar strInfoLog[1024] = { 0 };
            glGetShaderInfoLog(shader, sizeof(strInfoLog) - 1, nullptr, strInfoLog);
            std::cerr << "\n>>> Shader compiler errors: \n"
                      << strInfoLog << std::endl;
        }
    }

    void debug_draw_renderer::linkProgram(const GLuint program)
    {
        glLinkProgram(program);
        checkGLError(__FILE__, __LINE__);

        GLint status;
        glGetProgramiv(program, GL_LINK_STATUS, &status);
        checkGLError(__FILE__, __LINE__);

        if (status == GL_FALSE) {
            GLchar strInfoLog[1024] = { 0 };
            glGetProgramInfoLog(program, sizeof(strInfoLog) - 1, nullptr, strInfoLog);
            std::cerr << "\n>>> Program linker errors: \n"
                      << strInfoLog << std::endl;
        }
    }
}
}