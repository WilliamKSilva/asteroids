CC = g++ 
CFLAGS = -Wall
BUILD = ./build/
RAYLIB_PATH=/usr/local/lib/
FILES = ./src/*.cpp
asteroids:
	$(CC) -o $(BUILD)asteroids $(FILES) $(RAYLIB_PATH)libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11
