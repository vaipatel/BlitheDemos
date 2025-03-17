#version 330 core

in vec4 col;
in vec2 TexCoords;
out vec4 FragColor1; // Pointed to by GL_COLOR_ATTACHMENT0 + 0. Will write to texture at m_handles[0] in texture unit GL_TEXTURE0 + 0.
uniform sampler2D myTex;
uniform bool useColorOverride;
uniform vec4 colorOverride;

void main()
{
    if ( useColorOverride )
    {
        FragColor1 = mix(colorOverride, mix(col, texture(myTex, TexCoords), 0.35), 0.5);
    }
    else
    {
        FragColor1 = mix(col, texture(myTex, TexCoords), 0.35);
    }
}
