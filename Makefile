ROOT=./

CFLAGS:=
LFLAGS:=
LIBS:=

ifeq ($(OS),Windows_NT)
	CFLAGS:= -Wall -O3 -g $(shell pkg-config --cflags sdl2 SDL2_ttf glew zlib)
	LFLAGS:= -g
	LIBS:= $(shell pkg-config --libs sdl2 SDL2_ttf glew zlib) -lopengl32 -lm -mconsole
else
#	UNAME_S := $(shell uname -s)
#	ifeq ($(UNAME_S),Linux)
		CFLAGS:= -g -O3 -Wall $(shell pkg-config --cflags sdl2 SDL2_ttf gl glew zlib) -fsanitize=address --no-omit-frame-pointer
		LFLAGS:= -g -fsanitize=address
		LIBS:= -lm $(shell pkg-config --libs sdl2 SDL2_ttf gl glew zlib) -pthread
#	endif
#	ifeq ($(UNAME_S),Darwin)
#	endif
endif

SRCDIR=$(ROOT)src/
LIBDIR=$(ROOT)lib/
BUILDDIR=$(ROOT)build/
OUTPUTDIR=$(ROOT)bin/

SRC_CPP:=$(wildcard $(SRCDIR)*.cpp)
SRC:=$(wildcard $(SRCDIR)*.c)

_OBJS=$(patsubst $(SRCDIR)%.c,%.o,$(SRC))
OBJS=$(patsubst %,$(BUILDDIR)%,$(_OBJS))

_OBJS_CPP=$(patsubst $(SRCDIR)%.cpp,%.o,$(SRC_CPP))
OBJS_CPP=$(patsubst %,$(BUILDDIR)%,$(_OBJS_CPP))

NAME=blocks

all:	$(OUTPUTDIR)$(NAME)

-include $(OBJS:.o=.d)
-include $(OBJS_CPP:.o=.d)

$(BUILDDIR)%.d:	$(SRCDIR)%.cpp
	$(CXX) $(CFLAGS) -M $< |  sed 's,$*\.o[ :]*,\$(BUILDDIR)$*\.o : ,g' > $@

$(BUILDDIR)%.o:	$(SRCDIR)%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(BUILDDIR)%.o:	$(SRCDIR)%.c
	$(CXX) $(CFLAGS) -c $< -o $@

$(OUTPUTDIR)$(NAME): $(OBJS_CPP) $(OBJS)
	$(CXX) $(LFLAGS) -o $(OUTPUTDIR)$(NAME) $(OBJS_CPP) $(OBJS) $(LIBS)

check-syntax:
	$(CXX) -s -o /dev/null -S $(CHK_SOURCES)

run:	$(OUTPUTDIR)$(NAME)
	$(OUTPUTDIR)$(NAME)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)* $(OUTPUTDIR)blocks
