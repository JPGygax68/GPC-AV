#version 430 

/** This shader offers the minimal modelview and projection transformations
    that used to be part of the OpenGL fixed pipeline.
 */

layout(location = 0) uniform mat4 modelview_matrix;
layout(location = 1) uniform mat4 projection_matrix;

in vec4 vertex;

void main() {

    gl_Position = projection_matrix * modelview_matrix * vertex;
}
