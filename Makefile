CC = g++ 
CFLAGS = -Wall
BUILD = ./build/
RAYLIB_PATH=/usr/local/lib/
asteroids:
	$(CC) -o $(BUILD)asteroids game.c -g $(RAYLIB_PATH)libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11
