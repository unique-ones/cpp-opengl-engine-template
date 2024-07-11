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

/// Retrieves the layout of the vertex
VertexBufferLayout Vertex::layout() {
    return { ShaderType::FLOAT2, ShaderType::FLOAT2, ShaderType::FLOAT4 };
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
      sprite_group("assets/vertex.glsl", "assets/sprite_fragment.glsl"),
      transform(1.0f) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

/// Begins a new render pass
void Renderer::begin(s32 width, s32 height) {
    viewport_width = width;
    viewport_height = height;
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

/// Draws a quad
void Renderer::draw_quad(const QuadCreateInfo &info) {
    RenderCommand command{};
    command.vertices = {
        Vertex{ { info.position.x, info.position.y }, {}, info.color },
        Vertex{ { info.position.x, info.position.y + info.size.y }, {}, info.color },
        Vertex{ { info.position.x + info.size.x, info.position.y + info.size.y }, {}, info.color },
        Vertex{ { info.position.x + info.size.x, info.position.y }, {}, info.color },
    };

    auto offset = static_cast<u32>(quad_group.commands.size() * 4);
    command.indices = { 0 + offset, 1 + offset, 2 + offset, 2 + offset, 0 + offset, 3 + offset };
    quad_group.push(command);
}

/// Draws a symbol
void Renderer::draw_symbol(const SymbolCreateInfo &info) {
    auto scale = info.size / GlyphCache::FONT_SIZE;
    auto scaled_size = info.glyph->size * scale;
    auto scaled_position = glm::vec2{ info.position.x + info.glyph->bearing.x * scale,
                                      info.position.y + (info.glyph->size.y - info.glyph->bearing.y) * scale };

    RenderCommand command{};
    command.vertices = {
        Vertex{ { scaled_position.x, scaled_position.y }, { info.glyph->texture_offset, 0.0f }, info.color },
        Vertex{ { scaled_position.x, scaled_position.y + scaled_size.y },
                { info.glyph->texture_offset, info.glyph->texture_span.y },
                info.color },
        Vertex{ { scaled_position.x + scaled_size.x, scaled_position.y + scaled_size.y },
                { info.glyph->texture_offset + info.glyph->texture_span.x, info.glyph->texture_span.y },
                info.color },
        Vertex{ { scaled_position.x + scaled_size.x, scaled_position.y },
                { info.glyph->texture_offset + info.glyph->texture_span.x, 0.0f },
                info.color }
    };

    auto offset = static_cast<u32>(glyph_group.commands.size() * 4);
    command.indices = { 0 + offset, 1 + offset, 2 + offset, 2 + offset, 0 + offset, 3 + offset };
    glyph_group.push(command);
}

/// Draws text
void Renderer::draw_text(const TextCreateInfo &info) {
    auto scale = info.size / GlyphCache::FONT_SIZE;
    auto iterator = info.position;
    auto symbol_info = [&](GlyphInfo &glyph) { return SymbolCreateInfo{ &glyph, iterator, info.color, info.size }; };

    for (auto ch : info.text) {
        switch (ch) {
            case '\n': {
                iterator.x = info.position.x;
                iterator.y += info.size;
                break;
            }
            case '\t': {
                auto glyph = cache.acquire(' ');
                for (u32 i = 0; i < 4; ++i) {
                    draw_symbol(symbol_info(glyph));
                    iterator.x += glyph.advance.x * scale;
                }
                break;
            }
            default: {
                auto glyph = cache.acquire(ch);
                draw_symbol(symbol_info(glyph));
                iterator.x += glyph.advance.x * scale;
            }
        }
    }
}

/// Draws a sprite
void Renderer::draw_sprite(const SpriteCreateInfo &info) {
    // Due to the fact that draw_sprite is not compatible with batch rendering,
    // it is necessary to restart the current render pass in order to preserve
    // draw order.
    // TODO(plank): Figure out a better approach
    end();
    begin(viewport_width, viewport_height);

    RenderCommand command{};
    command.vertices = {
        Vertex{ { info.position.x, info.position.y }, { 0, 0 }, info.color },
        Vertex{ { info.position.x, info.position.y + info.size.y }, { 0, 1 }, info.color },
        Vertex{ { info.position.x + info.size.x, info.position.y + info.size.y }, { 1, 1 }, info.color },
        Vertex{ { info.position.x + info.size.x, info.position.y }, { 1, 0 }, info.color },
    };
    command.indices = { 0, 1, 2, 2, 0, 3 };

    // Push the command to the sprite group
    sprite_group.push(command);

    // Configure location of sprite texture
    info.texture->bind(0);
    sprite_group.shader.uniform("uniform_texture", 0);

    // Perform draw call
    end_internal(sprite_group);

    // Cleanup
    Texture::unbind(0);
    sprite_group.clear();
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
    auto vertices_size = 4 * sizeof(Vertex);
    auto indices_size = 6 * sizeof(u32);
    std::vector<Vertex> vertices(vertices_size * command_count);
    std::vector<u32> indices(indices_size * command_count);

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
