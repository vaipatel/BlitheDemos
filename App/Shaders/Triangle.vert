#version 330 core

in highp vec3 VertexPosition;
in lowp vec4 VertexColor;
in vec2 VertexTexCoords;
out lowp vec4 col;
out vec2 TexCoords;
uniform highp mat4 matrix;

void main()
{
   col = VertexColor;
   gl_Position = matrix * vec4(VertexPosition.x, VertexPosition.y, VertexPosition.z, 1.0);
   TexCoords = VertexTexCoords;
}
