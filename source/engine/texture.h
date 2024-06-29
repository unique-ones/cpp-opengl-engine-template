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

#ifndef ENGINE_TEXTURE_H
#define ENGINE_TEXTURE_H

#include "types.h"

struct Texture {
    u32 handle;
    s32 width;
    s32 height;
    s32 channels;
    u8 *data;

    Texture() = default;

    /// Loads a texture from the given path and uploads it to the gpu
    /// @param path The path to the image file
    explicit Texture(const fs::path &path);

    /// Destroys the specified texture and its data
    ~Texture();

    /// Binds the texture to the sampler at the specified slot
    /// @param slot The sampler slot
    void bind(u32 slot) const;

    /// Unbinds the currently bound texture at the specified sampler slot
    /// @param slot The sampler slot
    static void unbind(u32 slot);
};

#endif// ENGINE_TEXTURE_H
