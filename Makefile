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
DEPS = lib/libglfw3.a include/GLFW include/stb_image.h

format:
	clang-format -i src/main.c

main: $(DEPS) $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) $(LDLIBS)

launch: main
	./main

glfw-$(GLFW_VERSION).zip:
	wget -nv https://github.com/glfw/glfw/releases/download/$(GLFW_VERSION)/glfw-$(GLFW_VERSION).zip

glfw-$(GLFW_VERSION): glfw-$(GLFW_VERSION).zip
	unzip -qq $<

lib/libglfw3.a: glfw-$(GLFW_VERSION)
	cd glfw-$(GLFW_VERSION) && cmake -S . -B build && make -C build
	mkdir -p lib
	cp glfw-$(GLFW_VERSION)/build/src/libglfw3.a lib

include/GLFW: glfw-$(GLFW_VERSION)
	mkdir -p include
	cp -r glfw-$(GLFW_VERSION)/include/GLFW include/

include/stb_image.h:
	wget -nv https://github.com/nothings/stb/raw/master/stb_image.h -O $@
