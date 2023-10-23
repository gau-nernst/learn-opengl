#shader vertex
#version 330 core

layout(location=0) in vec4 position;
layout(location=1) in vec4 v_color;
out vec4 f_color;

void main() {
  gl_Position = position;
  f_color = v_color;
}

#shader fragment
#version 330 core

in vec4 f_color;
out vec4 color;

void main() {
  color = f_color;
}
