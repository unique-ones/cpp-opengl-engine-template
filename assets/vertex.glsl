#version 450 core
layout (location = 0) in vec2 attrib_position;
layout (location = 1) in vec2 attrib_texture_coordinates;
layout (location = 2) in vec4 attrib_color;

layout (location = 0) out vec4 passed_color;
layout (location = 1) out vec2 passed_texture_coordinates;

uniform mat4 uniform_transform;

void main() {
    gl_Position = uniform_transform * vec4(attrib_position, 0.0, 1.0);
    passed_texture_coordinates = attrib_texture_coordinates;
    passed_color = attrib_color;
}
