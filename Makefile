CFLAGS = -Wall

BUILD = ./build/

OBJS = $(BUILD)main.o

RAYLIB_PATH=/usr/local/lib/

asteroids:
	$(CC) -o $(BUILD)asteroids main.c $(RAYLIB_PATH)libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11
