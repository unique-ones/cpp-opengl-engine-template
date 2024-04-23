#version 450 core
layout (location = 0) out vec4 fragment_color;
layout (location = 0) in vec4 passed_color;

void main() {
    fragment_color = passed_color;
}
