#include "random.h"

int randomNumber(int limit)
{
	srand(time(NULL));

	return (rand() % limit) + 1;
}