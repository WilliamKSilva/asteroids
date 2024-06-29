#ifndef NUMBER
#define NUMBER

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int randomNumber(int limit)
{
	srand(time(NULL));

	return (rand() % limit) + 1;
}

#endif