# Learn OpenGL

https://learnopengl.com/

Instructions below are for macOS.

Build GLFW

```bash
wget https://github.com/glfw/glfw/releases/download/3.3.8/glfw-3.3.8.zip
unzip glfw-3.3.8.zip
cd glfw-3.3.8
cmake -S . -B build
cd build
make
```

Copy `glfw-3.3.8/build/src/libglfw3.a` to `lib`, copy `glfw-3.3.8/include` to `include`.

Add GLAD to project (https://glad.dav1d.de/)
- Select API gl = Version 3.3, Profile = Core. Then generate
- Copy `include/glab` and `include/KHR` to `include`. Copy `src/glad.c` to `src`.

Compile and link

```bash
clang src/*.c -o main -Iinclude -Llib -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit
```
