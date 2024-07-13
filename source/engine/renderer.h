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
#include <set>

struct Vertex {
    glm::vec2 position;
    glm::vec4 color;
    glm::vec2 texture_coordinates;
    s32 texture_index;

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

struct QuadExtent {
    glm::vec2 position;
    glm::vec2 size;
};

struct SymbolExtent {
    glm::vec2 position;
    f32 size;
};

using TextExtent = SymbolExtent;

struct Renderer {
    GlyphCache cache;
    RenderGroup glyph_group;
    RenderGroup quad_group;
    glm::mat4 transform;

    constexpr static inline s32 TEXTURE_START = 1;
    constexpr static inline s32 TEXTURE_MAX = 32;
    std::unordered_map<u32, s32> textures;

    /// Creates a new renderer
    Renderer();

    /// Begins a new render pass
    /// @param width The width of the viewport
    /// @param height The height of the viewport
    void begin(s32 width, s32 height);

    /// Ends the started render pass and submits to the gpu
    void end();

    /// Draws a colored quad
    /// @param ext The quad's extent
    void draw_quad(const QuadExtent &ext, const glm::vec4 &color);

    /// Draws a textured quad
    /// @param ext The quad's extent
    void draw_quad(const QuadExtent &ext, const Texture &texture);

    /// Draws a symbol
    /// @param ext The symbol's extent
    void draw_symbol(const SymbolExtent &ext, const glm::vec4 &color, const GlyphInfo &glyph);

    /// Draws text
    /// @param ext The text's extent
    void draw_text(const TextExtent &ext, const glm::vec4 &color, std::string_view text);

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
