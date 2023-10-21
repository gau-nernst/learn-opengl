GLFW_VERSION = 3.3.8

CFLAGS += -Iinclude

LDFLAGS += -Llib
LDLIBS += -lglfw3
ifeq ($(shell uname -s),Linux)
LDLIBS += $(addprefix -l,m GL X11 pthread Xrandr Xi dl)
endif
ifeq ($(shell uname -s),Darwin)
LDLIBS += $(addprefix -framework ,OpenGL Cocoa IOKit)
endif

OBJECTS = src/main.o src/glad.o

format:
	clang-format -i src/main.c

main: $(OBJECTS)
	cc $(OBJECTS) -o $@ $(LDFLAGS) $(LDLIBS)

launch: main
	./main

glfw:
	wget -nv https://github.com/glfw/glfw/releases/download/$(GLFW_VERSION)/glfw-$(GLFW_VERSION).zip
	unzip -qq glfw-$(GLFW_VERSION).zip
	cd glfw-$(GLFW_VERSION) && cmake -S . -B build && make -C build
	mkdir -p lib
	cp glfw-$(GLFW_VERSION)/build/src/libglfw3.a lib
	mkdir -p include
	cp -r glfw-$(GLFW_VERSION)/include/GLFW include/
