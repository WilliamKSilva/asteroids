CC = gcc 
CFLAGS = -Wall
RAYLIB_PATH=/usr/local/lib/
FILES = ./src/*.c
BUILD = ./build/
asteroids:
	$(CC) -o $(BUILD)asteroids -g $(FILES) $(RAYLIB_PATH)libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11
