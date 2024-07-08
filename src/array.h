#ifndef ARRAY_H
#define ARRAY_H

#include <stdlib.h>

typedef struct {
  void *ptr;
  int length;
  size_t itemSize;
} Array;

void delete_from_array(Array *array, int index);

#endif
