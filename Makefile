GLFW_VERSION = 3.3.8

CFLAGS += -Wall -Iinclude -Istb -Iglfw-$(GLFW_VERSION)/include
LDFLAGS += -L.
LDLIBS += -lglfw3

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Linux)
LDLIBS += $(addprefix -l,m GL X11 pthread Xrandr Xi dl)
endif

ifeq ($(UNAME_S), Darwin)
LDLIBS += $(addprefix -framework ,OpenGL Cocoa IOKit)
endif

OBJECTS = src/main.o src/glad.o
DEPS = libglfw3.a

main: $(DEPS) $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) $(LDLIBS)

launch: main
	./main

# GLFW stuff
glfw-$(GLFW_VERSION).zip:
	wget -nv https://github.com/glfw/glfw/releases/download/$(GLFW_VERSION)/glfw-$(GLFW_VERSION).zip

libglfw3.a: glfw-$(GLFW_VERSION).zip
	-rm -rf glfw-$(GLFW_VERSION)
	unzip -qq glfw-$(GLFW_VERSION).zip
	cd glfw-$(GLFW_VERSION) && cmake -S . -B build && make -C build
	cp glfw-$(GLFW_VERSION)/build/src/libglfw3.a .

format:
	clang-format -i src/main.c

clean:
	rm $(OBJECTS)
