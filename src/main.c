#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define STBI_ASSERT(x)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

typedef unsigned int uint;

#define ASSERT(condition, ...)                                                                                         \
  if (!(condition)) {                                                                                                  \
    fprintf(stderr, ##__VA_ARGS__);                                                                                    \
    fprintf(stderr, "\n");                                                                                             \
    raise(SIGTRAP);                                                                                                    \
  }

#define GL_CALL(x)                                                                                                     \
  {                                                                                                                    \
    while (glGetError())                                                                                               \
      ;                                                                                                                \
    x;                                                                                                                 \
    uint error = GL_NO_ERROR;                                                                                          \
    while ((error = glGetError())) {                                                                                   \
      fprintf(stderr, "[OpenGL Error 0x%.04X] %s line %d\n", error, #x, __LINE__);                                     \
      break;                                                                                                           \
    }                                                                                                                  \
    ASSERT(error == GL_NO_ERROR, "");                                                                                  \
  }

typedef struct Vertex {
  float position[2];
  float color[3];
  float texture_coord[2];
} Vertex;

static uint compile_shader(uint type, const char *source);
static uint create_shader_program(const char *vertexShader, const char *fragmentShader);
static uint shader_from_file(const char *path);

int main(void) {
  if (!glfwInit())
    return -1;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // for macOS
#endif

  GLFWwindow *window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    return -1;
  }

  Vertex vertices[] = {
      -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, //
      -0.5f, 0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, //
      0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f, //
      0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, //
  };
  uint indices[] = {
      0, 1, 2, //
      2, 3, 0, //
  };

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); // not sure why it's S and T
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // upsample cannot use mipmap
  int width, height, n_channels;
  unsigned char *texture_data = stbi_load("wall.jpg", &width, &height, &n_channels, 0);
  ASSERT(texture_data != NULL, "Failed to load image");

  uint texture;
  GL_CALL(glGenTextures(1, &texture));
  GL_CALL(glBindTexture(GL_TEXTURE_2D, texture));
  GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data));
  GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
  stbi_image_free(texture_data);

  uint VAO, VBO, EBO;
  GL_CALL(glGenVertexArrays(1, &VAO));
  GL_CALL(glBindVertexArray(VAO)); // bind VAO first. this stores glVertexAttribPointer() info.

  GL_CALL(glGenBuffers(1, &VBO));
  GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
  GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

  // position attribute
  GL_CALL(glEnableVertexAttribArray(0));
  GL_CALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0));

  // color attribute
  GL_CALL(glEnableVertexAttribArray(1));
  GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(2 * sizeof(float))));

  // texture attribute
  GL_CALL(glEnableVertexAttribArray(2));
  GL_CALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)((2 + 3) * sizeof(float))));

  GL_CALL(glGenBuffers(1, &EBO));
  GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
  GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

  // unbind
  GL_CALL(glBindVertexArray(0));
  GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
  GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0)); // unbind EBO after VAO

  uint shader = shader_from_file("shaders/basic.glsl");
  GL_CALL(glUseProgram(shader));
  uint rotation2d_location;
  GL_CALL(rotation2d_location = glGetUniformLocation(shader, "rotation2d"));

  while (!glfwWindowShouldClose(window)) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, 1);

    GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    float angle = glfwGetTime();
    float rotation2d[] = {
        cos(angle), -sin(angle), //
        sin(angle), cos(angle),  //
    };
    GL_CALL(glUniformMatrix2fv(rotation2d_location, 1, GL_TRUE, rotation2d));

    GL_CALL(glBindVertexArray(VAO)); // this will also bind EBO
    // GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 3));
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);

    GL_CALL(glBindVertexArray(0));

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteProgram(shader);

  glfwTerminate();
  return 0;
}

static uint compile_shader(uint type, const char *source) {
  uint id = glCreateShader(type);
  glShaderSource(id, 1, &source, NULL);
  glCompileShader(id);

  int result;
  glGetShaderiv(id, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) {
    int length;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
    char *message = malloc(length * sizeof(char));
    glGetShaderInfoLog(id, length, &length, message);
    fprintf(stderr, "Failed to compile shader\n");
    fprintf(stderr, "%s\n", message);
    glDeleteShader(id);
    free(message);
    return 0;
  }

  return id;
}

static uint create_shader_program(const char *vertexShader, const char *fragmentShader) {
  uint program;
  GL_CALL(program = glCreateProgram());

  uint vs = compile_shader(GL_VERTEX_SHADER, vertexShader);
  uint fs = compile_shader(GL_FRAGMENT_SHADER, fragmentShader);

  GL_CALL(glAttachShader(program, vs));
  GL_CALL(glAttachShader(program, fs));
  GL_CALL(glLinkProgram(program));
  GL_CALL(glValidateProgram(program));

  GL_CALL(glDeleteShader(vs));
  GL_CALL(glDeleteShader(fs));
  return program;
}

static uint shader_from_file(const char *path) {
  FILE *f = fopen(path, "r");
  ASSERT(f != NULL, "Unable to open %s\n", path);
  ASSERT(fseek(f, 0, SEEK_END) == 0, "Unable to seek to end");
  long size = ftell(f);
  ASSERT(size != -1, "Error");

  char *source = malloc(sizeof(char) * (size + 1));
  ASSERT(fseek(f, 0, SEEK_SET) == 0, "Unable to seek to start");
  fread(source, sizeof(char), size, f);
  source[size + 1] = '\0';

  char vertex_tag[] = "#shader vertex\n";
  char *vertex = strstr(source, vertex_tag);
  ASSERT(vertex != NULL, "Can't find vertex shader in shader file");

  char fragment_tag[] = "#shader fragment\n";
  char *fragment = strstr(source, fragment_tag);
  ASSERT(fragment != NULL, "Can't find fragment shader in shader file");

  memset(vertex, 0, sizeof(vertex_tag) - 1);
  vertex += sizeof(vertex_tag) - 1;

  memset(fragment, 0, sizeof(fragment_tag) - 1);
  fragment += sizeof(fragment_tag) - 1;

  printf("=== VERTEX SHADER ===\n%s\n", vertex);
  printf("=== FRAGMENT SHADER ===\n%s\n", fragment);

  uint program = create_shader_program(vertex, fragment);
  free(source);
  return program;
}
