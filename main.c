#include <stdio.h>
#include <stdlib.h>
#include "hdd.h"

int main(int argc, char **argv)
{
	struct Sector *hdd;
	uint8_t lines = 0;

	hdd_init(&hdd, lines);

	return EXIT_SUCCESS;
}
