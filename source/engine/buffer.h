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

#ifndef ENGINE_BUFFER_H
#define ENGINE_BUFFER_H

#include "shader.h"

#include <memory>
#include <optional>
#include <vector>

using VertexBufferLayout = std::vector<ShaderType>;

struct VertexBuffer {
    u32 handle;
    VertexBufferLayout layout;

    /// Creates a vertex buffer on the gpu
    VertexBuffer();

    /// Destroys the vertex buffer
    ~VertexBuffer();

    /// Sets the data for the vertex buffer
    /// @param data The vertex data
    template<typename T>
    void submit(const std::vector<T> &data) {
        glBindBuffer(GL_ARRAY_BUFFER, handle);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), GL_DYNAMIC_DRAW);
    }

    /// Binds the vertex buffer
    void bind() const;

    /// Unbinds all vertex buffers
    static void unbind();
};

struct IndexBuffer {
    u32 handle;
    u32 count;

    /// Creates an index buffer on the gpu
    IndexBuffer();

    /// Destroys the index buffer
    ~IndexBuffer();

    /// Sets the data for the index buffer
    /// @param indices The indices
    void submit(const std::vector<u32> &indices);

    /// Binds the index buffer
    void bind() const;

    /// Unbinds all index buffers
    static void unbind();
};

struct VertexArray {
    u32 handle;
    VertexBuffer *vertex_buffer;
    IndexBuffer *index_buffer;

    /// Creates a new vertex array
    VertexArray();

    /// Destroys the vertex array
    ~VertexArray();

    /// Submits the vertex buffer to the vertex array, this sets all the specified attributes
    /// @param buffer The vertex buffer handle
    void submit(VertexBuffer *buffer);
    /// Submits the index buffer to the vertex array
    /// @param buffer The index buffer handle
    void submit(IndexBuffer *buffer);

    /// Binds the vertex array
    void bind() const;

    /// Unbinds all vertex arrays
    static void unbind();
};

struct FrameBufferInfo {
    s32 width;
    s32 height;
    s32 internal_format;
    u32 pixel_type;
    u32 pixel_format;
};

struct FrameBuffer {
    u32 handle;
    u32 texture_handle;
    u32 render_handle;
    FrameBufferInfo info;

    /// Creates a new frame buffer
    /// @param info The frame buffer specification
    explicit FrameBuffer(const FrameBufferInfo &info);

    /// Destroys the frame buffer
    ~FrameBuffer();

    /// Invalidates the frame buffer
    void invalidate();

    /// Resizes the frame buffer
    /// @param width The new width
    /// @param height The new height
    void resize(s32 width, s32 height);

    /// Binds the frame buffer for rendering
    void bind() const;

    /// Binds the texture of the frame buffer at the specified sampler slot
    /// @param slot The sampler slot
    void bind_texture(u32 slot) const;

    /// Unbinds the currently bound frame buffer
    static void unbind();
};

#endif// ENGINE_BUFFER_H
