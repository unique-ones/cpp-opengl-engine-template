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

#ifndef ENGINE_GLYPH_H
#define ENGINE_GLYPH_H

#include "texture.h"
#include "types.h"

struct GlyphInfo {
    glm::vec2 size;
    glm::vec2 bearing;
    glm::vec2 advance;
    glm::vec2 texture_span;
    f32 texture_offset;
};

struct GlyphCache {
    Texture atlas;
    GlyphInfo info[128];

    static inline constexpr auto FONT_SIZE = 24;

    /// Creates a glyph cache for the specified font
    explicit GlyphCache(const fs::path &path);

    /// Fetches the specified symbol from the glyph cache
    /// @param symbol The symbol that shall be fetched
    /// @return The glyph info handle where the data is placed into
    GlyphInfo &acquire(char symbol);
};

#endif// ENGINE_GLYPH_H
