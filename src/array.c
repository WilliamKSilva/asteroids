#include "array.h"

#include <stdio.h>
#include <string.h>

void delete_from_array(Array *array, int indexToDelete) {
  if (indexToDelete < 0 || indexToDelete >= array->length)
    return;

  array->length--;

  char *data = (char*)array->ptr;  

  char *dest = data + indexToDelete * array->itemSize;
  char *source = data + (indexToDelete + 1) * array->itemSize;
  size_t offset = (array->length - indexToDelete) * array->itemSize;

  memmove(dest, source, offset);

  printf("INFO: object deleted\n");
}
