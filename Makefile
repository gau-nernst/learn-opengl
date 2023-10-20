GLFW_VERSION = 3.3.8

LIBRARIES = glfw3
FRAMEWORKS = OpenGL Cocoa IOKit

%.o: %.c
	cc -c $< -o $@ -Iinclude

format:
	clang-format -i src/main.c

main: lib/libglfw3.a src/main.o
	cc $< -o main -Iinclude -Llib $(addprefix -l,$(LIBRARIES)) $(addprefix -framework ,$(FRAMEWORKS))

launch: main
	./main

glfw-$(GLFW_VERSION).zip:
	wget https://github.com/glfw/glfw/releases/download/$(GLFW_VERSION)/glfw-$(GLFW_VERSION).zip

lib/libglfw3.a: glfw-$(GLFW_VERSION).zip
	unzip glfw-$(GLFW_VERSION).zip
	cd glfw-$(GLFW_VERSION) && cmake -S . -B build && make -C build
	cp glfw-$(GLFW_VERSION)/build/src/libglfw3.a lib
	cp -r glfw-$(GLFW_VERSION)/include/* include
