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

#include "renderer.h"

#include <array>
#include <glm/gtc/matrix_transform.hpp>
#include <ranges>

namespace {

constexpr auto WHITE = glm::vec4(1.0f);
constexpr auto NO_TEXTURE = -1;

}// namespace

/// Retrieves the layout of the vertex
VertexBufferLayout Vertex::layout() {
    return { ShaderType::FLOAT2, ShaderType::FLOAT4, ShaderType::FLOAT2, ShaderType::INT };
}

/// Creates a new render group
RenderGroup::RenderGroup(const fs::path &vertex, const fs::path &fragment)
    : commands(),
      vertex_array(),
      vertex_buffer(),
      index_buffer(),
      shader(vertex, fragment) {
    vertex_buffer.layout = std::move(Vertex::layout());
    vertex_array.submit(&vertex_buffer);
    vertex_array.submit(&index_buffer);
}

/// Clears the specified render group (i.e. deletes the commands)
void RenderGroup::clear() {
    commands = {};
}

/// Pushes a set of vertices and indices to the render group
void RenderGroup::push(const RenderCommand &command) {
    commands.emplace_back(command);
}

/// Creates a new renderer
Renderer::Renderer()
    : cache("assets/cmu-serif-roman.ttf"),
      glyph_group("assets/vertex.glsl", "assets/glyph_fragment.glsl"),
      quad_group("assets/vertex.glsl", "assets/quad_fragment.glsl"),
      transform(1.0f) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Configure quad texture slots
    for (auto i = s32{ 0 }; i < TEXTURE_MAX; ++i) {
        auto name = std::format("uniform_textures[{}]", i);
        quad_group.shader.uniform(name.c_str(), i + TEXTURE_START);
    }

    // Configure glyph atlas slot
    glyph_group.shader.uniform("uniform_glyph_atlas", 0);
}

/// Begins a new render pass
void Renderer::begin(s32 width, s32 height) {
    glyph_group.clear();
    quad_group.clear();
    transform = glm::ortho(0.0f, static_cast<f32>(width), static_cast<f32>(height), 0.0f);
}

/// Ends the started render pass and submits to the gpu
void Renderer::end() {
    end_internal(quad_group);

    cache.atlas.bind(0);
    end_internal(glyph_group);
    Texture::unbind(0);
}

/// Draws a colored quad
void Renderer::draw_quad(const QuadExtent &ext, const glm::vec4 &color) {
    RenderCommand command{};
    command.vertices = {
        Vertex{ { ext.position.x, ext.position.y }, color, { 0, 0 }, NO_TEXTURE },
        Vertex{ { ext.position.x, ext.position.y + ext.size.y }, color, { 0, 1 }, NO_TEXTURE },
        Vertex{ { ext.position.x + ext.size.x, ext.position.y + ext.size.y }, color, { 1, 1 }, NO_TEXTURE },
        Vertex{ { ext.position.x + ext.size.x, ext.position.y }, color, { 1, 0 }, NO_TEXTURE },
    };

    auto offset = static_cast<u32>(quad_group.commands.size() * 4);
    command.indices = { 0 + offset, 1 + offset, 2 + offset, 2 + offset, 0 + offset, 3 + offset };
    quad_group.push(command);
}

/// Draws a textured quad
void Renderer::draw_quad(const QuadExtent &ext, const Texture &texture) {
    s32 index;
    if (textures.contains(texture.handle)) {
        index = textures[texture.handle];
    } else {
        if (textures.size() == TEXTURE_MAX) {
            end_internal(quad_group);
            quad_group.clear();
            textures.clear();
        }
        index = static_cast<s32>(textures.size());
        textures[texture.handle] = index;
    }
    texture.bind(index + TEXTURE_START);

    RenderCommand command{};
    command.vertices = {
        Vertex{ { ext.position.x, ext.position.y }, WHITE, { 0, 0 }, index },
        Vertex{ { ext.position.x, ext.position.y + ext.size.y }, WHITE, { 0, 1 }, index },
        Vertex{ { ext.position.x + ext.size.x, ext.position.y + ext.size.y }, WHITE, { 1, 1 }, index },
        Vertex{ { ext.position.x + ext.size.x, ext.position.y }, WHITE, { 1, 0 }, index },
    };

    auto offset = static_cast<u32>(quad_group.commands.size() * 4);
    command.indices = { 0 + offset, 1 + offset, 2 + offset, 2 + offset, 0 + offset, 3 + offset };
    quad_group.push(command);
}

/// Draws a symbol
void Renderer::draw_symbol(const SymbolExtent &ext, const glm::vec4 &color, const GlyphInfo &glyph) {
    auto scale = ext.size / GlyphCache::FONT_SIZE;
    auto scaled_size = glm::vec2{ glyph.size } * scale;
    auto scaled_position = glm::vec2{ ext.position.x + static_cast<f32>(glyph.bearing.x) * scale,
                                      ext.position.y + static_cast<f32>(glyph.size.y - glyph.bearing.y) * scale };

    RenderCommand command{};
    command.vertices = {
        Vertex{ { scaled_position.x, scaled_position.y }, color, { glyph.texture_offset, 0.0f }, NO_TEXTURE },
        Vertex{ { scaled_position.x, scaled_position.y + scaled_size.y },
                color,
                { glyph.texture_offset, glyph.texture_span.y },
                NO_TEXTURE },
        Vertex{ { scaled_position.x + scaled_size.x, scaled_position.y + scaled_size.y },
                color,
                { glyph.texture_offset + glyph.texture_span.x, glyph.texture_span.y },
                NO_TEXTURE },
        Vertex{ { scaled_position.x + scaled_size.x, scaled_position.y },
                color,
                { glyph.texture_offset + glyph.texture_span.x, 0.0f },
                NO_TEXTURE }
    };

    auto offset = static_cast<u32>(glyph_group.commands.size() * 4);
    command.indices = { 0 + offset, 1 + offset, 2 + offset, 2 + offset, 0 + offset, 3 + offset };
    glyph_group.push(command);
}

/// Draws text
void Renderer::draw_text(const TextExtent &ext, const glm::vec4 &color, std::string_view text) {
    auto scale = ext.size / GlyphCache::FONT_SIZE;
    auto iterator = ext.position;

    for (auto ch : text) {
        switch (ch) {
            case '\n': {
                iterator.x = ext.position.x;
                iterator.y += ext.size;
                break;
            }
            case '\t': {
                auto glyph = cache.acquire(' ');
                for (u32 i = 0; i < 4; ++i) {
                    draw_symbol({ iterator, ext.size }, color, glyph);
                    iterator.x += glyph.advance.x * scale;
                }
                break;
            }
            default: {
                auto glyph = cache.acquire(ch);
                draw_symbol({ iterator, ext.size }, color, glyph);
                iterator.x += glyph.advance.x * scale;
            }
        }
    }
}

/// Clears the currently bound frame buffer
void Renderer::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/// Sets the clear color
void Renderer::clear_color(const glm::vec4 &color) {
    glClearColor(color.r, color.g, color.b, color.a);
}

/// Ends the started render pass internally for the specified group and shader
void Renderer::end_internal(RenderGroup &group) {
    if (group.commands.empty()) {
        return;
    }

    auto command_count = group.commands.size();
    std::vector<Vertex> vertices(4 * command_count);
    std::vector<u32> indices(6 * command_count);

    usize insert_index = 0;
    for (auto &command : group.commands) {
        auto vertex_offset = insert_index * 4;
        vertices[vertex_offset + 0] = command.vertices[0];
        vertices[vertex_offset + 1] = command.vertices[1];
        vertices[vertex_offset + 2] = command.vertices[2];
        vertices[vertex_offset + 3] = command.vertices[3];

        auto index_offset = insert_index * 6;
        indices[index_offset + 0] = command.indices[0];
        indices[index_offset + 1] = command.indices[1];
        indices[index_offset + 2] = command.indices[2];
        indices[index_offset + 3] = command.indices[3];
        indices[index_offset + 4] = command.indices[4];
        indices[index_offset + 5] = command.indices[5];
        insert_index++;
    }

    group.vertex_buffer.submit(vertices);
    group.index_buffer.submit(indices);
    draw_indexed(group);
}

/// Performs the indexed draw call for the specified group
void Renderer::draw_indexed(RenderGroup &group) const {
    group.vertex_array.bind();
    group.shader.bind();
    group.shader.uniform("uniform_transform", transform);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(group.index_buffer.count), GL_UNSIGNED_INT, nullptr);
    Shader::unbind();
    VertexArray::unbind();
}
