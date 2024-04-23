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

#include "glyph.h"
#include "file.h"

// clang-format off
#include <ft2build.h>
#include <freetype/freetype.h>
#include <fstream>
#include <iterator>
// clang-format on

/// Creates a glyph cache for the specified font
GlyphCache::GlyphCache(const fs::path &path) {
    auto content = File::read(path, std::ios::binary);
    if (not content) {
        assert(false && "[glyph] Cannot load font!");
    }

    FT_Library library;
    if (FT_Init_FreeType(&library)) {
        assert(false && "[glyph] Cannot initialize FreeType!");
    }

    FT_Face face;
    if (FT_New_Memory_Face(library, (FT_Byte *) content->c_str(), (FT_Long) content->size(), 0, &face)) {
        FT_Done_FreeType(library);
        assert(false && "[glyph] Cannot allocate font memory for FreeType!");
    }
    FT_Set_Pixel_Sizes(face, 0, FONT_SIZE);

    // calculate combined size of glyphs
    glm::ivec2 size = { 0, 0 };
    for (s32 i = 32; i < 128; i++) {
        if (FT_Load_Char(face, (FT_ULong) i, FT_LOAD_RENDER)) {
            fprintf(stderr, "could not load character: %c\n", (char) i);
            continue;
        }

        GlyphInfo *info = (this->info + i - 32);
        info->size.x = (f32) (face->glyph->bitmap.width);
        info->size.y = (f32) (face->glyph->bitmap.rows);
        info->bearing.x = (f32) (face->glyph->bitmap_left);
        info->bearing.y = (f32) (face->glyph->bitmap_top);
        info->advance.x = (f32) (face->glyph->advance.x >> 6);
        info->advance.y = (f32) (face->glyph->advance.y >> 6);
        info->texture_span.x = 0.0f;
        info->texture_span.y = 0.0f;
        info->texture_offset = 0.0f;
        size.x += (s32) face->glyph->bitmap.width;
        size.y = std::max(size.y, (s32) face->glyph->bitmap.rows);
    }

    atlas.data = nullptr;
    atlas.handle = 0;
    atlas.width = size.x;
    atlas.height = size.y;
    atlas.channels = 1;
    glCreateTextures(GL_TEXTURE_2D, 1, &atlas.handle);
    glBindTextureUnit(0, atlas.handle);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, size.x, size.y, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);

    s32 offset = 0;
    for (u32 i = 0; i < 96; i++) {
        // unfortunately we still need to load the character again, as we need its bitmap buffer for the upload
        if (FT_Load_Char(face, i + 32, FT_LOAD_RENDER) or face->glyph->bitmap.buffer == NULL) {
            continue;
        }
        GlyphInfo *info = (this->info + i);
        info->texture_offset = (f32) offset / (f32) size.x;
        info->texture_span.x = info->size.x / (f32) size.x;
        info->texture_span.y = info->size.y / (f32) size.y;
        info->bearing.y -= (f32) size.y - info->size.y;
        glTexSubImage2D(GL_TEXTURE_2D, 0, offset, 0, (s32) info->size.x, (s32) info->size.y, GL_RED, GL_UNSIGNED_BYTE,
                        face->glyph->bitmap.buffer);
        offset += (s32) info->size.x;
    }

    FT_Done_Face(face);
    FT_Done_FreeType(library);
}

/// Fetches the specified symbol from the glyph cache
GlyphInfo &GlyphCache::acquire(char symbol) {
    return info[symbol - 32];
}
