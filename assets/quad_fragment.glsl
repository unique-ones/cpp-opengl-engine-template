#version 450 core
layout (location = 0) out vec4 fragment_color;
layout (location = 0) in vec4 passed_color;
layout (location = 1) in vec2 passed_texture_coordinates;
layout (location = 2) in flat int passed_texture_index;

uniform sampler2D uniform_textures[32];

void main() {
    vec4 texture_color = vec4(1.0);
    if (passed_texture_index != -1) {
        texture_color = texture(uniform_textures[passed_texture_index], passed_texture_coordinates);
    }
    fragment_color = passed_color * texture_color;
}
