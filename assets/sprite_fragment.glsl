#version 450 core
layout (location = 0) out vec4 out_color;

layout (location = 0) in vec4 passed_color;
layout (location = 1) in vec2 passed_texture_coordinates;

uniform sampler2D uniform_texture;

void main() {
    out_color = passed_color * texture(uniform_texture, passed_texture_coordinates);
}
