#version 300 es

in vec4 VertexPosition;
out vec4 v_GenertingUV;

void main() {
    gl_Position = VertexPosition;
    v_GenertingUV = VertexPosition;
}

