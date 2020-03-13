# Directories
INCDIR=include
SRCDIR=src
OBJDIR=obj
LIBDIR=lib/x64
# Dependencies
_DEPS = vec3f.hpp engine.hpp window.hpp camera.hpp scene.hpp geometry.hpp SDL2/SDL.h
_OBJ = vec3f.o engine.o window.o camera.o scene.o geometry.o main.o 

DEPS = $(patsubst %,$(INCDIR)/%,$(_DEPS))
OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))

# Compiler
CC=g++
CFLAGS=-I$(INCDIR) -std=c++17

# Stuff
LDFLAGS = -L $(LIBDIR) -l OpenCL -l SDL2main -l SDL2 -l pthread

# executable file rules
main: $(OBJ)
	$(CC) -o $@ $^ ${LDFLAGS}

# object file rules
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(DEPS) 
	$(CC) -c -o $@ $< $(CFLAGS)
$(OBJDIR)/main.o: main.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# clean up
clean:
	rm -f $(OBJDIR)/*.o main.exe