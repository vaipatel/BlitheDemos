#version 330 core

in vec4 col;
out vec4 FragColor1; // Pointed to by GL_COLOR_ATTACHMENT0 + 0. Will write to texture at m_handles[0] in texture unit GL_TEXTURE0 + 0.

void main()
{
   FragColor1 = col;
}