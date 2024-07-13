//
//  MIT License
//
//  Copyright (c) 2024 unique-ones
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  SOFTWARE.

#include "engine/renderer.h"
#include "engine/window.h"

int main(int argc, char **argv) {
    // Construct a window with the specified dimensions and title
    WindowCreateInfo window_info{};
    window_info.width = 800;
    window_info.height = 400;
    window_info.title = "OpenGL Renderer";
    Window window{ window_info };

    // Construct the 2D renderer
    Renderer renderer{};

    // Configure the clear color of the renderer to be dark grey
    Renderer::clear_color({ 0.15f, 0.15f, 0.15f, 1.0f });

    Texture white_knight{ "assets/wn.png" };
    Texture white_queen{ "assets/wq.png" };
    Texture white_rook{ "assets/wr.png" };

    // Continue event loop while the window wants to stay open
    while (not window.should_close()) {
        // Clear the viewport at the begin of the frame
        Renderer::clear();

        // Begin recording render commands
        renderer.begin(window.width, window.height);

        // Draw three quads
        QuadExtent red_extent{};
        red_extent.position = { 20.0f, 20.0f };
        red_extent.size = { 50.0f, 50.0f };
        renderer.draw_quad(red_extent, { 1.0f, 0.0f, 0.0f, 1.0f });
        renderer.draw_quad(red_extent, white_queen);

        QuadExtent green_extent{};
        green_extent.position = { 70.0f, 20.0f };
        green_extent.size = { 50.0f, 50.0f };
        renderer.draw_quad(green_extent, { 0.0f, 1.0f, 0.0f, 1.0f });
        renderer.draw_quad(green_extent, white_knight);

        QuadExtent blue_extent{};
        blue_extent.position = { 120.0f, 20.0f };
        blue_extent.size = { 50.0f, 50.0f };
        renderer.draw_quad(blue_extent, { 0.0f, 0.0f, 1.0f, 1.0f });
        renderer.draw_quad(blue_extent, white_rook);

        // Draw a sample text
        TextExtent text_extent{};
        text_extent.position = { 20.0f, 100.0f };
        text_extent.size = GlyphCache::FONT_SIZE;
        renderer.draw_text(text_extent, { 1.0f, 1.0f, 1.0f, 1.0f }, "The quick brown fox jumps over the lazy dog.");

        // End the render pass which ultimately submits the draw call to the GPU
        renderer.end();

        // Update the window in order to swap front and back buffers and
        window.update();
    }

    return 0;
}
