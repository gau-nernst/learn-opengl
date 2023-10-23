#shader vertex
#version 330 core

layout(location=0) in vec2 aPos;
layout(location=1) in vec4 aColor;
layout(location=2) in vec2 aTexCoord;

uniform mat2 rotation2d;

out vec4 f_color;
out vec2 TexCoord;

void main() {
  gl_Position = vec4(rotation2d * aPos, 0.0f, 1.0f);
  f_color = aColor;
  TexCoord = aTexCoord;
}

#shader fragment
#version 330 core

in vec4 f_color;
in vec2 TexCoord;

uniform sampler2D ourTexture;

out vec4 FragColor;

void main() {
  FragColor = f_color * texture(ourTexture, TexCoord); // texture() is a built-in function
}
