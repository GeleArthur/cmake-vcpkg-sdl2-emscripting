#version 300 es

precision highp float;
out vec4 outColor;
uniform sampler2D u_texture;
in vec2 v_GenertingUV;


void main()
{
    outColor = texture(u_texture, v_GenertingUV);
    outColor = vec4(1);
}
