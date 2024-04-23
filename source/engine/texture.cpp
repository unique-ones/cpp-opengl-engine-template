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

#include "texture.h"

#include <stb_image.h>

/// Loads a texture from the given path and uploads it to the gpu
Texture::Texture(const fs::path &path) : handle(0), width(0), height(0), channels(4) {
    glCreateTextures(GL_TEXTURE_2D, 1, &handle);
    glBindTextureUnit(0, handle);

    stbi_set_flip_vertically_on_load(0);

    auto native_path = path.string();
    data = stbi_load(native_path.c_str(), &width, &height, &channels, 4);
    if (not data) {
        assert(false and "[texture] Failed to allocate memory for texture!");
    }

    glTextureStorage2D(handle, 1, GL_RGBA8, width, height);
    glTextureParameteri(handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(handle, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTextureParameteri(handle, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTextureSubImage2D(handle, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
}

/// Destroys the specified texture and its data
Texture::~Texture() {
    free(data);
    glDeleteTextures(1, &handle);
}

/// Binds the texture to the sampler at the specified slot
void Texture::bind(u32 slot) const {
    glBindTextureUnit(slot, handle);
}

/// Unbinds the currently bound texture at the specified sampler slot
void Texture::unbind(u32 slot) {
    glBindTextureUnit(slot, 0);
}
