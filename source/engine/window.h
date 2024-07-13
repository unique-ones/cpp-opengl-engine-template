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

#ifndef ENGINE_WINDOW_H
#define ENGINE_WINDOW_H

#include "types.h"

#include <GLFW/glfw3.h>

struct WindowCreateInfo {
    s32 width;
    s32 height;
    const char *title;
};

struct Window {
    GLFWwindow *handle;
    s32 width;
    s32 height;

    /// Creates a new window
    /// @param info The window information
    explicit Window(const WindowCreateInfo &info);

    /// Destroys the window
    ~Window();

    /// Returns whether the window should close
    /// @return A boolean value that indicates whether the window should close
    bool should_close() const;

    /// Updates the window by swapping front and back buffers and polling events
    void update() const;
};

#endif// ENGINE_WINDOW_H
