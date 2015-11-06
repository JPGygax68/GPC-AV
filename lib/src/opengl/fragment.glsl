#version 430

//layout(location = 0) uniform int            render_mode;
layout(location = 0) uniform sampler2D      Y;
layout(location = 1) uniform sampler2D      U;
layout(location = 2) uniform sampler2D      V;

out vec4 fragment_color;

void main() {

    fragment_color = vec4(1, 1, 1, 1);
}