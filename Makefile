CC = gcc 
CFLAGS = -Wall -Wextra
RAYLIB_PATH=/usr/local/lib/
FILES = ./src/*.c
BUILD = ./build/
asteroids:
	$(CC) -o $(BUILD)asteroids $(FILES) -g $(CFLAGS) $(RAYLIB_PATH)libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11
