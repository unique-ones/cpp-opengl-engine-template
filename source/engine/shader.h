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

#ifndef ENGINE_SHADER_H
#define ENGINE_SHADER_H

#include "types.h"

enum class ShaderType {
    INT = 0,
    INT2,
    INT3,
    INT4,
    FLOAT,
    FLOAT2,
    FLOAT3,
    FLOAT4,
    SAMPLER = INT
};

struct Shader {
    u32 handle;
    std::unordered_map<std::string, u32> uniforms;

    /// Creates a shader from the given vertex and fragment shader files
    /// @param vertex path to the vertex shader
    /// @param fragment path to the fragment shader
    Shader(const fs::path &vertex, const fs::path &fragment);

    /// Destroys the specified shader
    ~Shader();

    /// Sets a s32 uniform
    /// @param name uniform name
    /// @param value value
    void uniform(const char *name, s32 value);

    /// Sets a u32 uniform
    /// @param name uniform name
    /// @param value value
    void uniform(const char *name, u32 value);

    /// Sets a glm::ivec2 uniform
    /// @param self shader handle
    /// @param name uniform name
    /// @param value value
    void uniform(const char *name, const glm::ivec2 &value);

    /// Sets a glm::ivec3 uniform
    /// @param self shader handle
    /// @param name uniform name
    /// @param value value
    void uniform(const char *name, const glm::ivec3 &value);

    /// Sets a glm::ivec4 uniform
    /// @param self shader handle
    /// @param name uniform name
    /// @param value value
    void uniform(const char *name, const glm::ivec4 &value);

    /// Sets a f32 uniform
    /// @param self shader handle
    /// @param name uniform name
    /// @param value value
    void uniform(const char *name, f32 value);

    /// Sets a glm::vec2 uniform
    /// @param self shader handle
    /// @param name uniform name
    /// @param value value
    void uniform(const char *name, const glm::vec2 &value);

    /// Sets a glm::vec3 uniform
    /// @param self shader handle
    /// @param name uniform name
    /// @param value value
    void uniform(const char *name, const glm::vec3 &value);

    /// Sets a glm::vec4 uniform
    /// @param self shader handle
    /// @param name uniform name
    /// @param value value
    void uniform(const char *name, const glm::vec4 &value);

    /// Sets a glm::mat4 uniform
    /// @param self shader handle
    /// @param name uniform name
    /// @param value value
    void uniform(const char *name, const glm::mat4 &value);

    /// Binds the shader
    void bind() const;

    /// Unbinds the currently bound shader
    static void unbind();

private:
    /// Retrieves the location of a uniform
    /// @param name The name of the uniform
    /// @return The location
    u32 uniform_location(const char *name);
};

#endif// ENGINE_SHADER_H
