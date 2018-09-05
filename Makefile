
CC = gcc
CFLAGS = -IC:\Users\ruben\Documents\Programs\SDL2-2.0.8\i686-w64-mingw32\include\SDL2
LDFLAGS = -LC:\Users\ruben\Documents\Programs\SDL2-2.0.8\i686-w64-mingw32\lib -lmingw32 -lSDL2main -lSDL2

run:
	${CC} ${CFLAGS} *.c -o build/chip8emu.exe ${LDFLAGS}

clean:
	del build/chip8emu.exe
