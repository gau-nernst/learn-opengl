GLFW_VERSION = 3.3.8

LIBRARIES = glfw3
LIB_FLAGS = $(addprefix -l,$(LIBRARIES))

FRAMEWORKS = OpenGL Cocoa IOKit
FRAMEWORK_FLAGS = $(addprefix -framework ,$(FRAMEWORKS))

%.o: %.c include/GLFW
	cc -c $< -o $@ -Iinclude

format:
	clang-format -i src/main.c

main: src/main.o lib/libglfw3.a
	cc $< -o $@ -Iinclude -Llib $(LIB_FLAGS) $(FRAMEWORK_FLAGS)

launch: main
	./main

glfw-$(GLFW_VERSION):
	wget -nv https://github.com/glfw/glfw/releases/download/$(GLFW_VERSION)/glfw-$(GLFW_VERSION).zip
	unzip -qq glfw-$(GLFW_VERSION).zip
	rm glfw-$(GLFW_VERSION).zip

lib/libglfw3.a: glfw-$(GLFW_VERSION)
	cd glfw-$(GLFW_VERSION) && cmake -S . -B build && make -C build
	mkdir -p lib
	cp glfw-$(GLFW_VERSION)/build/src/libglfw3.a lib

include/GLFW: glfw-$(GLFW_VERSION)
	mkdir -p include
	cp -r glfw-$(GLFW_VERSION)/include/GLFW include/
