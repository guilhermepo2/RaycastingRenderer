CC = gcc
SOURCE = ./src/main.c
OUTPUT = raycast
WINDOWS_LINKER_FLAGS = -lmingw32 -lSDL2main
SDL_LINKER_FLAGS = -lSDL2

# not using include and lib anymore, but I still want to go back and have SDL2 on the repo
SDL_INCLUDE = "./src/thirdparty/SDL2-2.0.12/i686-w64-mingw32/include"
SDL_LIB = "./src/thirdparty/SDL2-2.0.12/i686-w64-mingw32/lib"

build:
	$(CC) -std=c99 $(SOURCE) $(SDL_LINKER_FLAGS) -o $(OUTPUT)

windows-mingw:
	$(CC) -std=c99 $(SOURCE) -I$(SDL_INCLUDE) -L$(SDL_LIB) $(WINDOWS_LINKER_FLAGS) $(SDL_LINKER_FLAGS) -o $(OUTPUT).exe

run:
	./$(OUTPUT)

clean:
	rm $(OUTPUT)