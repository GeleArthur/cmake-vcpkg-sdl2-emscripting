#version 300 es

in vec4 VertexPosition;
out vec2 v_GenertingUV;

uniform mat4 u_matrix;

void main() {
    gl_Position = u_matrix * VertexPosition;
    v_GenertingUV = VertexPosition.xy * 0.1;
}
