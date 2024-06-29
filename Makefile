CC = g++ 
CFLAGS = -Wall
BUILD = ./build/
RAYLIB_PATH=/usr/local/lib/
asteroids:
	$(CC) -o $(BUILD)asteroids game.cpp gameObject.cpp asteroid.cpp random.cpp player.cpp projectile.cpp text.cpp timer.cpp $(RAYLIB_PATH)libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11
