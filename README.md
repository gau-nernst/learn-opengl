# Learn OpenGL

https://learnopengl.com/

Instructions below are for macOS.

Add GLAD to project (https://glad.dav1d.de/)
- Select API gl = Version 3.3, Profile = Core. Then generate
- Copy `include/glab` and `include/KHR` to `include`. Copy `src/glad.c` to `src`.

Compile and link

```bash
clang src/*.c -o main -Iinclude -Llib -lglfw3 -lglad -framework Cocoa -framework OpenGL -framework IOKit
```
