#version 330 core

in vec4 col;
in vec2 TexCoords;
out vec4 FragColor1; // Pointed to by GL_COLOR_ATTACHMENT0 + 0. Will write to texture at m_handles[0] in texture unit GL_TEXTURE0 + 0.
uniform sampler2D myTex;

void main()
{
   FragColor1 = mix(col, texture(myTex, TexCoords), 0.35);
}
