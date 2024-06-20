#ifndef ARRAY 
#define ARRAY 

#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int length;
  void** data;
  size_t bytes;
} Array;

Array *PushToArray(Array **array, void *value) {
  Array *newArr = (Array*)malloc(sizeof(Array)); 
  // Dont know a way yet to call sizeof for generic data type
  // so this approach works for now since we cant know the type
  // of the value that is being passed
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
