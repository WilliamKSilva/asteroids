#ifndef ARRAY 
#define ARRAY 

#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int length;
  void** data;
  size_t bytes;
} Array;

Array *PushAsteroid(Array **array, Asteroid *asteroid) {
  Array *newArr = (Array*)malloc(sizeof(Array)); 
  newArr->data = (void**)malloc((*array)->bytes);

  for (int i = 0; i < (*array)->length; ++i) {
    if ((*array)->data[i] == NULL)
      continue;

    newArr->data[i] = (*array)->data[i];
  }

  newArr->length = (*array)->length + 1;

  free(*array);
  *array = NULL;

  newArr->data[newArr->length - 1] = value;

  return newArr;
}

#endif
