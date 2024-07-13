//
// MIT License
//
// Copyright (c) 2024 Elias Engelbert Plank
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "shader.h"
#include "file.h"

#include <cstdio>
#include <string>

namespace {

/// Compiles the shader source
std::optional<u32> compile(const fs::path &path, u32 type) {
    auto source = File::read(path);
    if (not source) {
        return std::nullopt;
    }

    auto program = glCreateShader(type);
    auto *shader_source = static_cast<const GLchar *>(source->data());
    glShaderSource(program, 1, &shader_source, nullptr);
    glCompileShader(program);

    s32 success;
    glGetShaderiv(program, GL_COMPILE_STATUS, &success);
    if (not success) {
        s32 info_length;
        glGetShaderiv(program, GL_INFO_LOG_LENGTH, &info_length);

        std::string message(info_length, 0);
        glGetShaderInfoLog(program, info_length, &info_length, message.data());
        glDeleteProgram(program);
        std::fprintf(stderr, "[shader] Compilation failed: %s\n", message.data());
        return std::nullopt;
    }
    return program;
}


}// namespace

/// Creates a shader from the given vertex and fragment shader files
Shader::Shader(const fs::path &vertex, const fs::path &fragment) {
    auto vertex_program = compile(vertex, GL_VERTEX_SHADER);
    auto fragment_program = compile(fragment, GL_FRAGMENT_SHADER);

    if (not vertex_program) {
        assert(false and "[shader] Failed to compile vertex shader!");
    }
    if (not fragment_program) {
        assert(false and "[shader] Failed to compile fragment shader!");
    }

    auto handle = glCreateProgram();
    glAttachShader(handle, *vertex_program);
    glAttachShader(handle, *fragment_program);
    glLinkProgram(handle);

    s32 link_success;
    glGetProgramiv(handle, GL_LINK_STATUS, &link_success);
    if (not link_success) {
        s32 info_length;
        glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &info_length);

        std::string message(info_length, 0);
        glGetProgramInfoLog(handle, info_length, &info_length, message.data());
        glDeleteProgram(handle);
        glDeleteProgram(*vertex_program);
        glDeleteProgram(*fragment_program);

        std::fprintf(stderr, "[shader] Linking failed: %s\n", message.data());
        assert(false and "[shader] Failed to link programs!");
    }
    this->handle = handle;

    s32 uniform_count;
    glGetProgramiv(handle, GL_ACTIVE_UNIFORMS, &uniform_count);

    s32 uniform_length;
    glGetProgramiv(handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniform_length);

    if (uniform_count > 0 and uniform_length > 0) {
        std::string uniform_name(uniform_length, 0);
        for (s32 i = 0; i < uniform_count; ++i) {
            s32 length;
            s32 size;
            u32 type;

            glGetActiveUniform(handle, i, uniform_length, &length, &size, &type, uniform_name.data());
            auto location = glGetUniformLocation(handle, uniform_name.data());
            uniforms[uniform_name] = location;

            std::fprintf(stdout, "[shader] Uniform '%s' has location '%d'.\n", uniform_name.data(), location);
        }
    }
}

/// Destroys the specified shader
Shader::~Shader() {
    glDeleteProgram(handle);
}

/// Sets a s32 uniform
void Shader::uniform(const char *name, s32 value) {
    bind();
    glUniform1i(uniform_location(name), value);
}

/// Sets a u32 uniform
void Shader::uniform(const char *name, u32 value) {
    bind();
    glUniform1ui(uniform_location(name), value);
}

/// Sets a glm::ivec2 uniform
void Shader::uniform(const char *name, const glm::ivec2 &value) {
    bind();
    glUniform2i(uniform_location(name), value.x, value.y);
}

/// Sets a glm::ivec3 uniform
void Shader::uniform(const char *name, const glm::ivec3 &value) {
    bind();
    glUniform3i(uniform_location(name), value.x, value.y, value.z);
}

/// Sets a glm::ivec4 uniform
void Shader::uniform(const char *name, const glm::ivec4 &value) {
    bind();
    glUniform4i(uniform_location(name), value.x, value.y, value.z, value.w);
}

/// Sets a f32 uniform
void Shader::uniform(const char *name, f32 value) {
    bind();
    glUniform1f(uniform_location(name), value);
}

/// Sets a glm::vec2 uniform
void Shader::uniform(const char *name, const glm::vec2 &value) {
    bind();
    glUniform2f(uniform_location(name), value.x, value.y);
}

/// Sets a glm::vec3 uniform
void Shader::uniform(const char *name, const glm::vec3 &value) {
    bind();
    glUniform3f(uniform_location(name), value.x, value.y, value.z);
}

/// Sets a glm::vec4 uniform
void Shader::uniform(const char *name, const glm::vec4 &value) {
    bind();
    glUniform4f(uniform_location(name), value.x, value.y, value.z, value.w);
}

/// Sets a glm::mat4 uniform
void Shader::uniform(const char *name, const glm::mat4 &value) {
    bind();
    glUniformMatrix4fv(uniform_location(name), 1, GL_FALSE, &value[0][0]);
}

/// Binds the shader
void Shader::bind() const {
    glUseProgram(handle);
}

/// Unbinds the currently bound shader
void Shader::unbind() {
    glUseProgram(0);
}

/// Retrieves the location of a uniform
s32 Shader::uniform_location(const char *name) {
    if (uniforms.contains(name)) {
        return uniforms[name];
    }
    auto location = glGetUniformLocation(handle, name);
    uniforms[name] = location;
    return location;
}
