CC = gcc 
CFLAGS = -Wall
RAYLIB_PATH=/usr/local/lib/
FILES = ./*.c
BUILD = ./build/
asteroids:
	$(CC) -o $(BUILD)asteroids $(FILES) $(RAYLIB_PATH)libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11
