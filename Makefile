EXE_TARGET  := gw
EXE_SRCS    := \
	src/main.cpp      \
	src/math.cpp      \
	src/ff_tga.cpp    \
	src/ff_wav.cpp    \
	src/ll_audio.cpp  \
	src/ll_input.cpp  \
	src/ll_image.cpp  \
	src/ll_shader.cpp \
	src/ll_sprite.cpp \
	src/display.cpp   \
	src/input.cpp     \
	src/entity.cpp

EXE_OBJS    := ${EXE_SRCS:.cpp=.o}
EXE_DEPS    := ${EXE_SRCS:.cpp=.dep}
EXE_CCFLAGS  = -I. -Iinclude -fstrict-aliasing -O3 -Wall -Wextra -ggdb -DGLEW_STATIC -DGL_EXTERNAL -DGL_USE_GLEW
EXE_LDFLAGS  = -Llib
EXE_LIBS     = -lstdc++ -lm -lglfw3 -lglew -framework Cocoa -framework OpenGL -framework OpenAL -framework IOKit -framework CoreVideo

.PHONY: all clean distclean game

all:: ${EXE_TARGET}

${EXE_TARGET}: ${EXE_OBJS}
	${CC} ${EXE_LDFLAGS} -o $@ $^ ${EXE_LIBS}

${EXE_OBJS}: %.o: %.cpp %.dep
	${CC} ${EXE_CCFLAGS} -o $@ -c $<

${EXE_DEPS}: %.dep: %.cpp Makefile
	${CC} ${EXE_CCFLAGS} -MM $< > $@

game:: ${EXE_TARGET}

clean::
	-rm -f *~ *.o *.dep src/*.o src/*.dep ${EXE_TARGET}

distclean:: clean

