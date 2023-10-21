#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned int uint;
#define MAX_SHADER_SIZE 256

#define assert(condition, ...)                                                                                         \
  if (!(condition)) {                                                                                                  \
    fprintf(stderr, ##__VA_ARGS__);                                                                                    \
    fprintf(stderr, "\n");                                                                                             \
    return 0;                                                                                                          \
  }

static uint compile_shader(uint type, const char *source);
static uint shader_from_file(uint type, const char *path);
static uint create_shader_program(const char *vertexShader, const char *fragmentShader);

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

  float vertices[] = {
      -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, //
      -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, //
      0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, //
  };

  uint VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO); // bind VAO first. this stores glVertexAttribPointer() info.

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, 5 * 3 * sizeof(float), vertices, GL_STATIC_DRAW);

  // position attribute
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);

  // color attribute
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(2 * sizeof(float)));

  // unbind
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind EBO after VAO, since VAO will remember the last binded EBO.

  uint shader = create_shader_program("shaders/vertex.glsl", "shaders/fragment.glsl");
  glUseProgram(shader);

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, 1);

    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glBindVertexArray(VAO); // this will also bind EBO
    glDrawArrays(GL_TRIANGLES, 0, 3);
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

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

static uint shader_from_file(uint type, const char *path) {
  FILE *f = fopen(path, "r");
  assert(f != NULL, "Unable to open %s\n", path);
  assert(fseek(f, 0, SEEK_END) == 0, "Unable to seek to end");
  long size = ftell(f);
  assert(size != -1, "Error");

  char *source = malloc(sizeof(char) * (size + 1));
  assert(fseek(f, 0, SEEK_SET) == 0, "Unable to seek to start");
  fread(source, sizeof(char), size, f);
  source[size + 1] = '\0';

  printf("%s Shader\n", type == GL_VERTEX_SHADER ? "Vertex" : "Fragment");
  printf("%s", source);

  fclose(f);
  uint id = compile_shader(type, source);
  free(source);
  return id;
}

static uint create_shader_program(const char *vertexShader, const char *fragmentShader) {
  uint program = glCreateProgram();
  uint vs = shader_from_file(GL_VERTEX_SHADER, vertexShader);
  uint fs = shader_from_file(GL_FRAGMENT_SHADER, fragmentShader);

  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);
  glValidateProgram(program);

  glDeleteShader(vs);
  glDeleteShader(fs);

  return program;
}
