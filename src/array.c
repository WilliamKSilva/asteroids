#include "array.h"
#include <stdio.h>
#include <string.h>

void delete_from_array(Array *array, int index) 
{
  if (index < 0 || index >= array->length)
    return;

  array->length--;

  char *data = (char*)array->ptr;

  char *dest = data + index * array->itemSize;
  char *source = data + (index + 1) * array->itemSize;
  size_t offset = (array->length - index) * array->itemSize;

  memmove(dest, source, offset);

  printf("INFO: object deleted\n");
}
