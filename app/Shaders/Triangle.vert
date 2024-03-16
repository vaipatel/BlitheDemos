#version 330 core

in highp vec3 VertexPosition;
in lowp vec4 VertexColor;
out lowp vec4 col;
//uniform highp mat4 matrix;

void main()
{
   col = VertexColor;
//   gl_Position = matrix * vec4(VertexPosition.x, VertexPosition.y, VertexPosition.z, 1.0);
   gl_Position = vec4(VertexPosition.x, VertexPosition.y, VertexPosition.z, 1.0);
}