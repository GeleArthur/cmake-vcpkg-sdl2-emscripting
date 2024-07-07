#version 300 es

precision highp float;
out vec4 outColor;
uniform sampler2D u_texture;
in vec4 v_GenertingUV;

void main()
{
    outColor = texture(u_texture, v_GenertingUV.xy + vec2(0.3, 0.1));
}
