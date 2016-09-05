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
		CFLAGS:= -g -O3 -Wall $(shell pkg-config --cflags sdl2 SDL2_ttf gl glew zlib)
		LFLAGS:= -g
		LIBS:= -lm $(shell pkg-config --libs sdl2 SDL2_ttf gl glew zlib) -pthread
#	endif
#	ifeq ($(UNAME_S),Darwin)
#	endif
endif

SRCDIR=$(ROOT)src/
LIBDIR=$(ROOT)lib/
BUILDDIR=$(ROOT)build/
OUTPUTDIR=$(ROOT)bin/

SRC:=$(wildcard $(SRCDIR)*.cpp)

_OBJS=$(patsubst $(SRCDIR)%.cpp,%.o,$(SRC))
OBJS=$(patsubst %,$(BUILDDIR)%,$(_OBJS))

NAME=blocks

all:	$(OUTPUTDIR)$(NAME)

-include $(OBJS:.o=.d)

$(BUILDDIR)%.d:	$(SRCDIR)%.cpp
	$(CXX) $(CFLAGS) -M $< |  sed 's,$*\.o[ :]*,\$(BUILDDIR)$*\.o : ,g' > $@

$(BUILDDIR)%.o:	$(SRCDIR)%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(OUTPUTDIR)$(NAME): $(OBJS)
	$(CXX) $(LFLAGS) -o $(OUTPUTDIR)$(NAME) $(OBJS) $(LIBS)

check-syntax:
	$(CXX) -s -o /dev/null -S $(CHK_SOURCES)

run:	$(OUTPUTDIR)$(NAME)
	$(OUTPUTDIR)$(NAME)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)* $(OUTPUTDIR)blocks

