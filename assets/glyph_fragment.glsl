#version 450 core
layout (location = 0) out vec4 fragment_color;
layout (location = 0) in vec4 passed_color;
layout (location = 1) in vec2 passed_texture_coordinates;

uniform sampler2D uniform_glyph_atlas;

void main() {
    fragment_color = passed_color * vec4(1.0, 1.0, 1.0, texture(uniform_glyph_atlas, passed_texture_coordinates).r);
}
