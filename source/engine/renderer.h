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

#ifndef ENGINE_RENDERER_H
#define ENGINE_RENDERER_H

#include "buffer.h"
#include "glyph.h"
#include "types.h"

#include <array>
#include <list>

struct Vertex {
    glm::vec2 position;
    glm::vec2 texture_coordinates;
    glm::vec4 color;

    /// Retrieves the layout of the vertex
    /// @return The layout
    static VertexBufferLayout layout();
};

struct RenderCommand {
    std::array<Vertex, 4> vertices;
    std::array<u32, 6> indices;
};

struct RenderGroup {
    std::list<RenderCommand> commands;
    VertexArray vertex_array;
    VertexBuffer vertex_buffer;
    IndexBuffer index_buffer;
    Shader shader;

    /// Creates a new render group
    /// @param vertex The vertex shader path
    /// @param fragment The fragment shader path
    RenderGroup(const fs::path &vertex, const fs::path &fragment);

    /// Clears the specified render group (i.e. deletes the commands)
    void clear();

    /// Pushes a render command to the render group
    void push(const RenderCommand &command);
};

struct QuadCreateInfo {
    glm::vec2 position;
    glm::vec2 size;
    glm::vec4 color;
};

struct SymbolCreateInfo {
    GlyphInfo *glyph;
    glm::vec2 position;
    glm::vec4 color;
    f32 size;
};

struct TextCreateInfo {
    std::string_view text;
    glm::vec2 position;
    glm::vec4 color;
    f32 size;
};

struct SpriteCreateInfo {
    Texture *texture;
    glm::vec2 position;
    glm::vec2 size;
    glm::vec4 color;
};

struct Renderer {
    GlyphCache cache;
    RenderGroup glyph_group;
    RenderGroup quad_group;
    RenderGroup sprite_group;

    s32 viewport_width;
    s32 viewport_height;
    glm::mat4 transform;

    /// Creates a new renderer
    Renderer();

    /// Begins a new render pass
    /// @param width The width of the viewport
    /// @param height The height of the viewport
    void begin(s32 width, s32 height);

    /// Ends the started render pass and submits to the gpu
    void end();

    /// Draws a quad
    /// @param info The quad's information
    void draw_quad(const QuadCreateInfo &info);

    /// Draws a symbol
    /// @param info The symbol's information
    void draw_symbol(const SymbolCreateInfo &info);

    /// Draws text
    /// @param info The text information
    void draw_text(const TextCreateInfo &info);

    /// Draws a sprite
    /// @param info The sprite information
    void draw_sprite(const SpriteCreateInfo &info);

    /// Clears the currently bound frame buffer
    static void clear();

    /// Sets the clear color
    /// @param color The color value for clear calls
    static void clear_color(const glm::vec4 &color);

private:
    /// Ends the started render pass internally for the specified group
    void end_internal(RenderGroup &group);

    /// Performs the indexed draw call for the specified group
    void draw_indexed(RenderGroup &group) const;
};

#endif// ENGINE_RENDERER_H
