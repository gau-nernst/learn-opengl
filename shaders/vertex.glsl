#version 330 core

layout(location=0) in vec4 position; // index of the attribute = 0
layout(location=1) in vec4 v_color;
out vec4 f_color;

void main() {
  gl_Position = position;
  f_color = v_color;
}
