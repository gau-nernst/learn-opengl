GLFW_VERSION = 3.3.8

LDFLAGS = -Llib -lglfw3
ifeq ($(shell uname -s),Linux)
LDFLAGS += $(addprefix -l,m GL X11 pthread Xrandr Xi dl)
endif
ifeq ($(shell uname -s),Darwin)
LDFLAGS += $(addprefix -framework ,OpenGL Cocoa IOKit)
endif

%.o: %.c include/GLFW
	cc -c $< -o $@ -Iinclude

format:
	clang-format -i src/main.c

main: lib/libglfw3.a src/main.o
	cc src/main.o -o $@ -Iinclude $(LDFLAGS)

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
