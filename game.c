#include "stdlib.h"
#include "raylib.h"
#include <math.h>
#include <stdio.h>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080 

#define PLAYER_ROTATION_SPEED 4.0
#define PLAYER_IMPULSE 1.0
#define PLAYER_MAX_IMPULSE 10.0
#define PLAYER_DRAG 0.5

#define PROJECTILE_SPEED 15.0
#define PROJECTILE_START_POSITION_SCALE 70 

// TODO: draw my own assets

typedef struct {
  Texture2D sprite;
  Rectangle source;
  Rectangle dest;
  Vector2 origin;
  float rotation;
  Color color;
} TexturePro;

typedef struct {
  TexturePro texture;
  float speed;
} Player;

typedef struct {
  TexturePro texture;
} Projectile;

typedef struct {
  Projectile *ptr;
  int length;
} ProjectileArray;

TexturePro buildTexturePro(Vector2 startPosition, const char *spritePath) {
  Texture2D sprite = LoadTexture(spritePath);
  TexturePro texture = {
    .color = WHITE,
    .source = {
      .x = 0.0,
      .y = 0.0,
      .width = (float)sprite.width,
      .height = (float)sprite.height
    },
    .dest = {
      .x = startPosition.x,
      .y = startPosition.y,
      .width = (float)sprite.width,
      .height = (float)sprite.height
    },
    .sprite = sprite,
    .origin = {
      .x =  (float)sprite.width / 2.0,
      .y = (float)sprite.height / 2.0
    },
    .rotation = 0.0
  };

  return texture;
}

void renderTexturePro(TexturePro texture) {
  DrawTexturePro(
    texture.sprite,
    texture.source,
    texture.dest,
    texture.origin,
    texture.rotation,
    texture.color 
  );
}

void movePlayer(Player *player) {
  if (IsKeyDown(KEY_A))
    player->texture.rotation -= PLAYER_ROTATION_SPEED;

  if (IsKeyDown(KEY_D))
    player->texture.rotation += PLAYER_ROTATION_SPEED;

  if (IsKeyDown(KEY_W)) {
    if (player->speed <= PLAYER_MAX_IMPULSE)
      player->speed += PLAYER_IMPULSE;

    player->texture.dest.x += sin(player->texture.rotation * DEG2RAD) * player->speed;
    player->texture.dest.y -= cos(player->texture.rotation * DEG2RAD) * player->speed;
  }

  if (IsKeyUp(KEY_W)) {
    bool isDraggingBackwards = player->speed - PLAYER_DRAG < 0; 

    if (isDraggingBackwards) {
      player->speed = 0.0;
      player->texture.dest.x += sin(player->texture.rotation * DEG2RAD) * player->speed;
      player->texture.dest.y -= cos(player->texture.rotation * DEG2RAD) * player->speed;
      return;
    }

    player->speed -= PLAYER_DRAG;
    player->texture.dest.x += sin(player->texture.rotation * DEG2RAD) * player->speed;
    player->texture.dest.y -= cos(player->texture.rotation * DEG2RAD) * player->speed;
  }
}

void spawnProjectile(ProjectileArray *projectiles, Vector2 startPosition, float rotation) {
  Projectile projectile = { 
    .texture = buildTexturePro(startPosition, "./assets/projectile.png")
  };

  projectile.texture.rotation = rotation;

  if (projectiles->length == 0) {
    projectiles->ptr = malloc(sizeof(Projectile));

    if (projectiles->ptr == NULL) {
      printf("Error trying to allocate memory for projectiles\n");
      exit(0);
    }

    projectiles->ptr[0] = projectile;
    projectiles->length++;
  } else {
    projectiles->length++;
    projectiles->ptr = realloc(projectiles->ptr, projectiles->length * sizeof(Projectile));

    if (projectiles->ptr == NULL) {
      printf("Error trying to allocate memory for projectiles\n");
      exit(0);
    }

    projectiles->ptr[projectiles->length - 1] = projectile;
  }
}

Vector2 getProjectileStartPosition(Player player) {
  Vector2 position = {
    .x = player.texture.dest.x,
    .y = player.texture.dest.y,
  };
  position.x += sin(player.texture.rotation * DEG2RAD) * PROJECTILE_START_POSITION_SCALE;
  position.y -= cos(player.texture.rotation * DEG2RAD) * PROJECTILE_START_POSITION_SCALE;

  return position;
}

void shootProjectile(ProjectileArray *projectiles, Player player) {
  if (IsKeyPressed(KEY_SPACE)) {
    spawnProjectile(projectiles, getProjectileStartPosition(player), player.texture.rotation);
  }
}

void moveProjectile(Projectile *projectile) {
  projectile->texture.dest.x += sin(projectile->texture.rotation * DEG2RAD) * PROJECTILE_SPEED;
  projectile->texture.dest.y -= cos(projectile->texture.rotation * DEG2RAD) * PROJECTILE_SPEED;
}

bool isObjectOutOfBounds(Vector2 position) {
  if (position.x > SCREEN_WIDTH || position.x < 0)
      return true;

  if (position.y > SCREEN_HEIGHT || position.y < 0)
      return true;

  return false;
}

void deleteProjectile(ProjectileArray *projectiles, int projectileIndex) {
  if (projectiles->length == 1) {
    free(projectiles->ptr);
    projectiles->ptr = NULL;
    projectiles->length = 0;
    printf("INFO: array with one element freed\n");
    return;
  }

  for (int i = projectileIndex; i < projectiles->length - 1; ++i) {
    projectiles->ptr[i] = projectiles->ptr[i + 1];
  }

  projectiles->length--;
  printf("INFO: object deleted\n");
}

void update(Player *player, ProjectileArray *projectiles) {
  // Input updates
  movePlayer(player);
  shootProjectile(projectiles, *player);

  // Scripted updates
  for (int i = 0; i < projectiles->length; ++i) {
    moveProjectile(&projectiles->ptr[i]); 

    Projectile projectile = projectiles->ptr[i];
    Vector2 projectilePosition = {
      .x = projectile.texture.dest.x,
      .y = projectile.texture.dest.y
    };

    if (isObjectOutOfBounds(projectilePosition)) {
      deleteProjectile(projectiles, i);
    }
  }
}

void render(Player player, ProjectileArray projectiles) {
  renderTexturePro(player.texture);

  for (int i = 0; i < projectiles.length; ++i) {
    renderTexturePro(projectiles.ptr[i].texture); 
  }
}

int main() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Asteroids");
  SetTargetFPS(60);

  Vector2 playerStartPosition = { 
    .x = (float)SCREEN_WIDTH / 2.0,
    .y = (float)SCREEN_HEIGHT / 2.0
  };
  Player player = {
    .texture = buildTexturePro(playerStartPosition, "./assets/player.png"),
    .speed = 0.0
  };

  ProjectileArray projectiles = {
    .ptr = NULL,
    .length = 0,
  };

  while (!WindowShouldClose()) {
    update(&player, &projectiles);

    BeginDrawing(); 
      ClearBackground(BLACK);
      render(player, projectiles);
    EndDrawing();
  }

  return 0;
}
