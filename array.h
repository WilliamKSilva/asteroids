#ifndef ARRAY 
#define ARRAY 

#include <stdio.h>
#include <stdlib.h>
#include "game.h"

typedef struct {
  int length;
  void** data;
} Array;

Array *PushAsteroid(Array **asteroids, Asteroid *asteroid) {
  if (asteroids == NULL || *asteroids == NULL) {
    printf("%s\n", "Invalid input: asteroids is NULL");
    return NULL;
  }

  Array *newArr = (Array*)malloc(sizeof(Array)); 
  if (newArr == NULL) {
    printf("%s\n", "Error trying to allocate memory for asteroids array");
    return NULL;
  }

  newArr->length = (*asteroids)->length + 1;
  newArr->data = malloc(newArr->length * sizeof(Asteroid*));
  if (newArr->data == NULL) {
    printf("%s\n", "Error trying to allocate memory for asteroids array data");
    free(newArr);
    return NULL;
  }

  for (int i = 0; i < (*asteroids)->length; ++i) {
    newArr->data[i] = (*asteroids)->data[i];
  }

  newArr->data[newArr->length - 1] = asteroid;

  free((*asteroids)->data);
  free(*asteroids);
  *asteroids = NULL;

  return newArr;
}

Array *RemoveAsteroid(Array **asteroids, int index) {
  if (asteroids == NULL || *asteroids == NULL || index < 0 || index >= (*asteroids)->length) {
    printf("%s\n", "Invalid input: asteroids is NULL or index is out of bounds");
    return NULL;
  }

  Array *newArr = (Array*)malloc(sizeof(Array)); 
  if (newArr == NULL) {
    printf("%s\n", "Error trying to allocate memory for asteroids array");
    return NULL;
  }

  newArr->length = (*asteroids)->length - 1;
  newArr->data = malloc(newArr->length * sizeof(Asteroid*));
  if (newArr->data == NULL) {
    printf("%s\n", "Error trying to allocate memory for asteroids array data");
    free(newArr);
    return NULL;
  }

  for (int i = 0, j = 0; i < (*asteroids)->length; ++i) {
    if (i == index) {
      continue;
    }
    newArr->data[j++] = (*asteroids)->data[i];
  }

  free((*asteroids)->data);
  free(*asteroids);
  *asteroids = NULL;

  return newArr;
}

#endif
