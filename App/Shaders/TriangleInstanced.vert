#version 330 core

in highp vec3 VertexPosition;
in lowp vec4 VertexColor;
in vec2 VertexTexCoords;
in mat4 InstanceMatrix;
out lowp vec4 col;
out vec2 TexCoords;
uniform highp mat4 viewProjection;

void main()
{
   col = VertexColor;
   gl_Position = viewProjection * InstanceMatrix * vec4(VertexPosition.x, VertexPosition.y, VertexPosition.z, 1.0);
   TexCoords = VertexTexCoords;
}
