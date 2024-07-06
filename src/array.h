#ifndef ARRAY_H
#define ARRAY_H

#include <stdlib.h>

typedef struct {
  void *ptr;
  int length;
  size_t itemSize;
} Array;

void deleteFromArray(Array *array, int indexToDelete);

#endif
