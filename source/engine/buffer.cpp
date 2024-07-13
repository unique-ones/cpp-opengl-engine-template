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

#include <cassert>

#include "buffer.h"

namespace {

/// Converts a shader type to its stride in bytes
s32 shader_type_stride(ShaderType type) {
    switch (type) {
        case ShaderType::INT:
            return sizeof(GLint);
        case ShaderType::INT2:
            return 2 * sizeof(GLint);
        case ShaderType::INT3:
            return 3 * sizeof(GLint);
        case ShaderType::INT4:
            return 4 * sizeof(GLint);
        case ShaderType::FLOAT:
            return sizeof(float);
        case ShaderType::FLOAT2:
            return 2 * sizeof(float);
        case ShaderType::FLOAT3:
            return 3 * sizeof(float);
        case ShaderType::FLOAT4:
            return 4 * sizeof(float);
        default:
            return 0;
    }
}

/// Converts a shader type to its OpenGL base type
s32 shader_type_opengl(ShaderType type) {
    switch (type) {
        case ShaderType::INT:
        case ShaderType::INT2:
        case ShaderType::INT3:
        case ShaderType::INT4:
            return GL_INT;
        case ShaderType::FLOAT:
        case ShaderType::FLOAT2:
        case ShaderType::FLOAT3:
        case ShaderType::FLOAT4:
            return GL_FLOAT;
        default:
            return 0;
    }
}

/// Calculates the number of primitives in a shader type
s32 shader_type_primitives(ShaderType type) {
    switch (type) {
        case ShaderType::INT:
            return 1;
        case ShaderType::INT2:
            return 2;
        case ShaderType::INT3:
            return 3;
        case ShaderType::INT4:
            return 4;
        case ShaderType::FLOAT:
            return 1;
        case ShaderType::FLOAT2:
            return 2;
        case ShaderType::FLOAT3:
            return 3;
        case ShaderType::FLOAT4:
            return 4;
        default:
            return 0;
    }
}

/// Calculates the total stride of a vertex buffer layout
s32 vertex_buffer_layout_stride(const VertexBufferLayout &layout) {
    s32 stride = 0;
    for (auto attribute : layout) {
        stride += shader_type_stride(attribute);
    }
    return stride;
}

}// namespace

/// Creates a vertex buffer on the gpu
VertexBuffer::VertexBuffer() : handle(0), layout() {
    glGenBuffers(1, &handle);
    glBindBuffer(GL_ARRAY_BUFFER, handle);
}

/// Destroys the vertex buffer
VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &handle);
}

/// Binds the vertex buffer
void VertexBuffer::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, handle);
}

/// Unbinds the currently bound vertex buffer
void VertexBuffer::unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/// Creates an index buffer on the gpu
IndexBuffer::IndexBuffer() : handle(0), count(0) {
    glGenBuffers(1, &handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
}

/// Destroys the index buffer
IndexBuffer::~IndexBuffer() {
    glDeleteBuffers(1, &handle);
}

/// Sets the data for the specified index buffer
void IndexBuffer::submit(const std::vector<u32> &indices) {
    auto size = static_cast<GLsizeiptr>(indices.size() * sizeof(u32));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices.data(), GL_DYNAMIC_DRAW);
    count = indices.size();
}

/// Binds the specified buffer
void IndexBuffer::bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
}

/// Unbinds the currently bound index buffer
void IndexBuffer::unbind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

/// Creates a new vertex array
VertexArray::VertexArray() : handle(0), vertex_buffer(nullptr), index_buffer(nullptr) {
    glGenVertexArrays(1, &handle);
    glBindVertexArray(handle);
}

/// Destroys the vertex array
VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &handle);
}

/// Sets the vertex buffer for the vertex array, this sets all the specified attributes
void VertexArray::submit(VertexBuffer *buffer) {
    this->bind();
    buffer->bind();

    s64 offset = 0;
    auto stride = vertex_buffer_layout_stride(buffer->layout);
    for (u32 i = 0; i < buffer->layout.size(); ++i) {
        glEnableVertexAttribArray(i);
        auto attribute = buffer->layout[i];
        auto opengl_type = shader_type_opengl(attribute);
        auto primitives = shader_type_primitives(attribute);
        if (opengl_type == GL_FLOAT) {
            glVertexAttribPointer(i, primitives, opengl_type, GL_FALSE, stride, reinterpret_cast<const void *>(offset));
        } else if (opengl_type == GL_INT) {
            glVertexAttribIPointer(i, primitives, opengl_type, stride, reinterpret_cast<const void *>(offset));
        }
        offset += shader_type_stride(attribute);
    }
    vertex_buffer = buffer;
}

/// Sets the index buffer for the vertex array
void VertexArray::submit(IndexBuffer *buffer) {
    this->bind();
    buffer->bind();
    index_buffer = buffer;
}

/// Binds the vertex array
void VertexArray::bind() const {
    glBindVertexArray(handle);
}

/// Unbinds the currently bound vertex array
void VertexArray::unbind() {
    glBindVertexArray(0);
}

/// Creates a new frame buffer
FrameBuffer::FrameBuffer(const FrameBufferInfo &info) : handle(0), texture_handle(0), render_handle(0), info(info) {
    invalidate();
}

/// Destroys the frame buffer
FrameBuffer::~FrameBuffer() {
    glDeleteFramebuffers(1, &handle);
    glDeleteTextures(1, &texture_handle);
    glDeleteRenderbuffers(1, &render_handle);
}

/// Invalidates the frame buffer, this needs to be called whenever the frame buffer is resized
void FrameBuffer::invalidate() {
    if (handle) {
        glDeleteFramebuffers(1, &handle);
        glDeleteTextures(1, &texture_handle);
        glDeleteRenderbuffers(1, &render_handle);
    }

    glGenFramebuffers(1, &handle);
    glBindFramebuffer(GL_FRAMEBUFFER, handle);

    glGenTextures(1, &texture_handle);
    glBindTexture(GL_TEXTURE_2D, texture_handle);
    glTexImage2D(GL_TEXTURE_2D, 0, info.internal_format, info.width, info.height, 0, info.pixel_format, info.pixel_type,
                 nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_handle, 0);

    glGenRenderbuffers(1, &render_handle);
    glBindRenderbuffer(GL_RENDERBUFFER, render_handle);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, info.width, info.height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, render_handle);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        assert(false and "[framebuffer] Invalid frame buffer!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/// Resizes the frame buffer
void FrameBuffer::resize(s32 width, s32 height) {
    if (width <= 0 or height <= 0 or (width == info.width and height == info.height)) {
        return;
    }

    info.width = width;
    info.height = height;
    invalidate();
}

/// Binds the specified frame buffer for rendering
void FrameBuffer::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, handle);
    glViewport(0, 0, info.width, info.height);
}

/// Binds the texture of the frame buffer at the specified sampler slot
void FrameBuffer::bind_texture(u32 slot) const {
    glBindTextureUnit(slot, texture_handle);
}

/// Unbinds the currently bound frame buffer
void FrameBuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
